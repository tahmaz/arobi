import numpy as np

# Sigmoid activation function
def sigmoid(x):
    #return 1/(1+np.exp(-x))
    #print("Sigmoid x: {0}".format(x))
    #print("Sigmoid: {0}, {1}".format(x, (1.0 / (1.0 + np.exp(-x)))))
    #print('{0:.6f}'.format(x, (1.0 / (1.0 + np.exp(-x)))))
    return 1.0 / (1.0 + np.exp(-x))
    #return x / (1.0 + np.mod(x))
    #if ( x>0 ):
    #    #print("Sigmoid: {0}, {1}".format(x, (1.0 / (1.0 + np.exp(-x)))))
    #    return 1 / (1 + np.exp(-x))
    #else:
        #print("Sigmoid: {0}, {1}".format(x, (1.0 / (1.0 + np.exp(x)))))
    #    return 1 / (1 + np.exp(x))


# Derivative of sigmoid activation function
def sigmoid_derivative(x):
    return x * (1 - x)
    #if (x > 0):
     #   return x * (1 - x)
    #else:
     #   return x * (1 + x)

#for i in range(-100,100):
#    sigmoid(i)
#    print("Sigmoid: {0}, {1}".format(i, sigmoid(i)))

# Input data
#x = np.array([[i] for i in range(100)])
#x = np.array([ [0.1], [0.2], [0.25], [0.35], [0.5], [0.62], [0.75], [0.8], [0.99] ])
x = np.array([ [1], [20], [25], [35], [50], [62], [75], [80], [99] ])
#x = np.array([10, 20, 25, 35, 50, 62, 75, 80, 99 ])
#print("x: {0}".format(x))
#print("a: {0}".format(a))

# Output data
#y = np.array([[1] if i < 50 else [0] for i in range(100)])
y = np.array([ [0], [0], [0], [0], [1], [1], [1], [1], [1] ])
#y = np.array([0, 0, 0, 0, 1, 1, 1, 1, 1 ])

# Seed the random number generator
np.random.seed(1)

# Initialize the weights randomly with mean 0
weights = 2*np.random.random((1, 1)) - 1

# Training loop
for i in range(1):
    # Forward pass
    layer0 = x
    # layer1 = sigmoid(np.dot(layer0, weights))
    layer1 = np.array([[sigmoid(np.dot(a[0], weights[0][0]))] for a in layer0])
    #for a in x :
    #    layer1.put ([sigmoid(np.dot(a[0], weights[0][0]))])
    #    print("a: {0}".format(a[0]))
    #    print("dot: {0}".format(np.dot(a[0], weights)[0][0]))
    #print("dot: {0}".format(np.dot(layer0, weights)))
    #print("weights: {0}".format(weights))
    #layer1 = layer1.T
    print("layer1: {0}".format(layer1.T))

    # Backward pass
    #layer1_error = y - layer1
    for a,b in zip(y,layer1):
        print("a: {0}, b: {1}, a-b: {2}".format(a[0],b[0],a[0]-b[0]))
    layer1_error = np.array([[(a[0]-b[0])] for a,b in zip(y,layer1)])
    print("layer1_error: {0}".format(layer1_error.T))
    #layer1_delta = layer1_error * sigmoid_derivative(layer1)
    layer1_delta = np.array([[a[0] * sigmoid_derivative(b[0])]  for a,b in zip(layer1_error,layer1)])
    print("layer1_delta: {0}".format(layer1_delta.T))

    # Update weights
    #weights += np.dot(layer0.T, layer1_delta)
    for a, b in zip(layer0, layer1_delta):
        weights += np.dot(a, b)
        print("weights: {0}".format(weights[0][0]))

# Test the model
print(layer1)

print(sigmoid(np.dot(np.array([[0.4]]), weights))) # should return close to 0
print(sigmoid(np.dot(np.array([[0.7]]), weights))) # should return close to 1

print(np.argmax(layer1, axis = 1))

#===============================================================
"""
# sigmoid function 0 ve 1 arasinda bir deyer alir hemishe, deriv=true tepe noqtesi 0 olan menfi ededlerdir /\
def nonlin(x, deriv=False):
    if (deriv == True):
        return x * (1 - x)
    return 1 / (1 + np.exp(-x))

# input dataset
X = np.array([ 10, 20, 25, 35, 50, 62, 75, 80, 99 ]).T

# output dataset .T sertlerle sutunlarin yerini deyishir, burada bir setr yaradir sonra onu sutuna cevirir. Birbasha sutunda yaratmaq olar(array([[0], [0], [1], [1]]))
y = np.array([ 0, 0, 0, 0, 1, 1, 1, 1, 1 ]).T

# seed random numbers to make calculation
# deterministic (just a good practice), neticesi sadece null-dur
#print(np.random.seed(1))

# A = np.random.random((3, 1) 3 stun, 1 setr olaraq 0-1 arasinda random eded generate et
# 2 * A - 1 hemin generate olunan 0-1 arasindaki ededleri -1 - 1 arasina cevir
# initialize weights randomly with mean 0, -1 ve 1 arasinda 3 eded 1 setrli matrix generate edir( np.random.random((3, 1))- 0-1 arasinda olan matrix) 2-ye vurub 1 cixdiqda deyer -1 ve 1 arasinda alinir
syn0 = 2 * np.random.random() - 1
print("Syn0: {0}".format(syn0))

#print("nonlim: {0}".format(nonlin(X.T)))

for iter in range(10):
    # forward propagation l0 girish matrixdir, l1 cixish. l0 ve generate olunmush -1 ve 1 arasinda olan martixsi bir-birine vurur ve alinan deyerleri 0-1 arasina cevirir
    l0 = X
    l1 = nonlin(np.dot(l0, syn0))
    print("l1: {0}".format(l1))

    # how much did we miss? dogru cavabdan biz tesadufi aldigimiz deyerleri cixir ve bizim sehvmizini tapiriq
    l1_error = y - l1
    print("l1_error: {0}".format(l1_error))

    # multiply how much we missed by the
    # slope of the sigmoid at the values in l1, sehvin deltasini tapiriq
    l1_delta = l1_error * nonlin(l1, True)
    print("l1_delta: {0}".format(l1_delta))

    # update weights, etdiyimiz sehv qeder dogruya yaxinlashiriq ve procedurur defelerle icra ederek dogruya yaxin netice almaga calishiriq
    syn0 += np.dot(l0, l1_delta)
    print("syn0: {0}".format(syn0))

print("Output After Training:")
print(l1)
"""