/* Copyright 2022 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.1
==============================================================================*/

#include "tensorflow/compiler/xla/service/gpu/runtime/cublas_lt_matmul.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "tensorflow/compiler/xla/mlir/runtime/transforms/custom_call_encoding.h"
#include "tensorflow/compiler/xla/runtime/custom_call.h"
#include "tensorflow/compiler/xla/runtime/executable.h"
#include "tensorflow/compiler/xla/runtime/logical_result.h"
#include "tensorflow/compiler/xla/runtime/state.h"
#include "tensorflow/compiler/xla/service/gpu/matmul_utils.h"
#include "tensorflow/compiler/xla/service/gpu/runtime/support.h"
#include "tensorflow/compiler/xla/service/service_executable_run_options.h"
#include "tensorflow/compiler/xla/stream_executor/scratch_allocator.h"
#include "tensorflow/compiler/xla/xla.pb.h"
#include "tensorflow/tsl/platform/status.h"

#if GOOGLE_CUDA
#include "tensorflow/compiler/xla/stream_executor/cuda/cuda_blas_lt.h"
#endif  // GOOGLE_CUDA

namespace xla {
#if GOOGLE_CUDA

using llvm::ArrayRef;

using xla::runtime::CustomCall;
using xla::runtime::CustomCallAttrEncodingSet;
using xla::runtime::EnumAttrEncoding;
using xla::runtime::State;
using xla::runtime::StridedMemrefView;

namespace lmhlo_gpu = ::mlir::lmhlo_gpu;

//===----------------------------------------------------------------------===//
// Register cuBLASLt attributes decoding with the Xla runtime.
//===----------------------------------------------------------------------===//

namespace runtime {
XLA_RUNTIME_REGISTER_ENUM_ATTR_DECODING(se::cuda::BlasLt::Epilogue);
}  // namespace runtime

//===----------------------------------------------------------------------===//
// Encoding from MHLO attributes to Xla runtime enums.
//===----------------------------------------------------------------------===//

namespace gpu {

void PopulateCublasLtMatmulAttrEncoding(CustomCallAttrEncodingSet& encoding) {
  encoding.Add<EnumAttrEncoding<lmhlo_gpu::CublasLtMatmulEpilogueAttr,
                                lmhlo_gpu::CublasLtMatmulEpilogue,
                                se::cuda::BlasLt::Epilogue>>(
      [](lmhlo_gpu::CublasLtMatmulEpilogue value)
          -> se::cuda::BlasLt::Epilogue {
        return cublas_lt::AsBlasLtEpilogue(value).value();
      });
}

//===----------------------------------------------------------------------===//
// cuBLASLt matmul custom call implementation.
//===----------------------------------------------------------------------===//

static absl::Status CublasLtMatmulImpl(
    const ServiceExecutableRunOptions* run_options,
    const DebugOptions* debug_options, State<GemmConfig> gemm_config,
    State<cublas_lt::MatmulPlan> matmul_plan, StridedMemrefView a,
    StridedMemrefView b, StridedMemrefView c, StridedMemrefView d,
    std::optional<StridedMemrefView> bias, std::optional<StridedMemrefView> aux,
    int64_t algorithm, double alpha_real, double alpha_imag, double beta,
    DotDimensionNumbers dot_dims, se::cuda::BlasLt::Epilogue epilogue,
    ArrayRef<int32_t> precision) {
  VLOG(3) << "Running CublasLtMatmul";
  se::Stream* stream = run_options->stream();

  // Find the gemm config for this instance of matmul.
  absl::StatusOr<GemmConfig*> config = gemm_config.GetOrCreate([&] {
    return ToAbsl(GetGemmConfig(a, b, c, algorithm, alpha_real, alpha_imag,
                                beta, dot_dims.lhs_batch, dot_dims.lhs_contract,
                                dot_dims.rhs_batch, dot_dims.rhs_contract));
  });
  if (!config.ok()) return config.status();

  // Get the matmul plan for this instance of matmul.
  absl::StatusOr<cublas_lt::MatmulPlan*> plan = matmul_plan.GetOrCreate(
      [&] { return ToAbsl(cublas_lt::MatmulPlan::From(**config, epilogue)); });
  if (!plan.ok()) return plan.status();

  auto algos = (*plan)->GetAlgorithms(stream);
  if (!algos.ok()) return ToAbslStatus(algos.status());

  se::DeviceMemoryBase a_data = GetDeviceAddress(a);
  se::DeviceMemoryBase b_data = GetDeviceAddress(b);
  se::DeviceMemoryBase c_data = GetDeviceAddress(c);
  se::DeviceMemoryBase d_data = GetDeviceAddress(d);
  se::DeviceMemoryBase bias_data;
  if (bias.has_value()) bias_data = GetDeviceAddress(*bias);
  se::DeviceMemoryBase aux_data;
  if (aux.has_value()) aux_data = GetDeviceAddress(*aux);

  se::OwningScratchAllocator<> scratch_allocator(
      stream->parent()->device_ordinal(), stream->parent()->GetAllocator());

  return ToAbslStatus((*plan)->ExecuteOnStream(
      stream, a_data, b_data, c_data, d_data, bias_data, aux_data,
      (*algos)[algorithm], scratch_allocator));
}

//===----------------------------------------------------------------------===//
// cuBLASLt custom calls bindings and registration.
//===----------------------------------------------------------------------===//

template <typename... Ts>
auto BindMatmulAttributes(runtime::CustomCallBinding<Ts...> binding) {
  return std::move(binding)
      .template Attr<int64_t>("algorithm")
      .template Attr<double>("alpha_real")
      .template Attr<double>("alpha_imag")
      .template Attr<double>("beta")
      .template Attr<DotDimensionNumbers>("dot_dims")
      .template Attr<se::cuda::BlasLt::Epilogue>("epilogue")
      .template Attr<ArrayRef<int32_t>>("precision");
}

auto CublasLtMatmulCall(const char* name) {
  return CustomCall::Bind(name)
      .UserData<const ServiceExecutableRunOptions*>()
      .UserData<const DebugOptions*>()
      .State<GemmConfig>("uid")
      .State<cublas_lt::MatmulPlan>("uid")
      .Arg<StridedMemrefView>()   // a
      .Arg<StridedMemrefView>()   // b
      .Arg<StridedMemrefView>()   // c
      .Arg<StridedMemrefView>();  // d
}

XLA_RUNTIME_DEFINE_CUSTOM_CALL(
    CublasLtMatmul, FunctionWrapper<CublasLtMatmulImpl>(), checks,
    BindMatmulAttributes(CublasLtMatmulCall("xla.gpu.cublas.lt.matmul")
                             .Value(std::optional<StridedMemrefView>())  // bias
                             .Value(std::optional<StridedMemrefView>())  // aux
                         ));

XLA_RUNTIME_DEFINE_CUSTOM_CALL(
    CublasLtMatmulBias, FunctionWrapper<CublasLtMatmulImpl>(), checks,
    BindMatmulAttributes(CublasLtMatmulCall("xla.gpu.cublas.lt.matmul.bias")
                             .Arg<StridedMemrefView>()                   // bias
                             .Value(std::optional<StridedMemrefView>())  // aux
                         ));

XLA_RUNTIME_DEFINE_CUSTOM_CALL(
    CublasLtMatmulAux, FunctionWrapper<CublasLtMatmulImpl>(), checks,
    BindMatmulAttributes(CublasLtMatmulCall("xla.gpu.cublas.lt.matmul.aux")
                             .Value(std::optional<StridedMemrefView>())  // bias
                             .Arg<StridedMemrefView>()                   // aux
                         ));

XLA_RUNTIME_DEFINE_CUSTOM_CALL(
    CublasLtMatmulBiasAux, FunctionWrapper<CublasLtMatmulImpl>(), checks,
    BindMatmulAttributes(CublasLtMatmulCall("xla.gpu.cublas.lt.matmul.bias.aux")
                             .Arg<StridedMemrefView>()  // bias
                             .Arg<StridedMemrefView>()  // aux
                         ));

void RegisterMatmulCustomCalls(runtime::DirectCustomCallRegistry& registry) {
  registry.Register("xla.gpu.cublas.lt.matmul", CublasLtMatmul);
  registry.Register("xla.gpu.cublas.lt.matmul.bias", CublasLtMatmulBias);
  registry.Register("xla.gpu.cublas.lt.matmul.aux", CublasLtMatmulAux);
  registry.Register("xla.gpu.cublas.lt.matmul.bias.aux", CublasLtMatmulBiasAux);
}

}  // namespace gpu
#endif  // GOOGLE_CUDA
}  // namespace xla
