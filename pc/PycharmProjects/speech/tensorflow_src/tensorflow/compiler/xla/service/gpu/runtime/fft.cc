/* Copyright 2022 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/compiler/xla/service/gpu/runtime/fft.h"

#include <memory>

#include "tensorflow/compiler/xla/mlir/runtime/transforms/custom_call_encoding.h"
#include "tensorflow/compiler/xla/runtime/custom_call.h"
#include "tensorflow/compiler/xla/runtime/executable.h"
#include "tensorflow/compiler/xla/runtime/state.h"
#include "tensorflow/compiler/xla/service/gpu/fft_thunk.h"
#include "tensorflow/compiler/xla/service/gpu/runtime/support.h"

namespace xla {
namespace gpu {

using xla::runtime::CustomCall;
using xla::runtime::State;
using xla::runtime::StridedMemrefView;

using llvm::ArrayRef;

namespace mhlo = ::mlir::mhlo;

namespace {
struct Fft {
  LLVM_ATTRIBUTE_ALWAYS_INLINE
  absl::Status operator()(const ServiceExecutableRunOptions* run_options,
                          State<std::unique_ptr<FftPlanCache>> state,
                          runtime::StridedMemrefView input,
                          runtime::StridedMemrefView output,
                          ArrayRef<int64_t> fft_length,
                          se::fft::Type fft_type) const;
  static Fft Handler() { return Fft(); }
};
}  // namespace

absl::Status Fft::operator()(const ServiceExecutableRunOptions* run_options,
                             State<std::unique_ptr<FftPlanCache>> state,
                             runtime::StridedMemrefView input,
                             runtime::StridedMemrefView output,
                             ArrayRef<int64_t> fft_length,
                             se::fft::Type fft_type) const {
  se::Stream* stream = run_options->stream();
  se::StreamExecutor* executor = stream->parent();

  if (input.dtype == PrimitiveType::F64 || input.dtype == PrimitiveType::C128) {
    // Adjust FFT type to reflect double precision.
    switch (fft_type) {
      case se::fft::Type::kC2CForward:
        fft_type = se::fft::Type::kZ2ZForward;
        break;
      case se::fft::Type::kC2CInverse:
        fft_type = se::fft::Type::kZ2ZInverse;
        break;
      case se::fft::Type::kR2C:
        fft_type = se::fft::Type::kD2Z;
        break;
      case se::fft::Type::kC2R:
        fft_type = se::fft::Type::kZ2D;
        break;
      default:
        return absl::InvalidArgumentError("Unsupported FFT type");
    }
  }

  absl::StatusOr<std::unique_ptr<FftPlanCache>*> fft_plan_cache =
      state.GetOrCreate([]() -> absl::StatusOr<std::unique_ptr<FftPlanCache>> {
        return std::make_unique<FftPlanCache>();
      });
  if (!fft_plan_cache.ok()) return fft_plan_cache.status();

  auto st =
      RunFft(GetDeviceAddress(input), ToShape(input), GetDeviceAddress(output),
             ToShape(output), fft_type, fft_length, executor->device_ordinal(),
             (*fft_plan_cache)->get(), stream, run_options->allocator());
  if (!st.ok()) return ToAbslStatus(st);

  return absl::OkStatus();
}

XLA_RUNTIME_DEFINE_CUSTOM_CALL(
    Fft, Fft::Handler(), checks,
    CustomCall::Bind("xla.gpu.fft")
        .UserData<const ServiceExecutableRunOptions*>()
        .State<std::unique_ptr<FftPlanCache>>("uid")
        .Arg<runtime::StridedMemrefView>()  // input
        .Arg<runtime::StridedMemrefView>()  // output
        .Attr<ArrayRef<int64_t>>("fft_length")
        .Attr<se::fft::Type>("fft_type"));

void PopulateFftAttrEncoding(runtime::CustomCallAttrEncodingSet& encoding) {
  encoding.Add<runtime::EnumAttrEncoding<mhlo::FftTypeAttr, mhlo::FftType,
                                         se::fft::Type>>(
      [](mhlo::FftType value) -> se::fft::Type {
        switch (value) {
          case mhlo::FftType::FFT:
            return se::fft::Type::kC2CForward;
          case mhlo::FftType::IFFT:
            return se::fft::Type::kC2CInverse;
          case mhlo::FftType::RFFT:
            return se::fft::Type::kR2C;
          case mhlo::FftType::IRFFT:
            return se::fft::Type::kC2R;
          default:
            return se::fft::Type::kInvalid;
        }
      });
}

void RegisterFftCustomCalls(runtime::DirectCustomCallRegistry& registry) {
  registry.Register("xla.gpu.fft", &xla::gpu::Fft);
}

}  // namespace gpu
}  // namespace xla
