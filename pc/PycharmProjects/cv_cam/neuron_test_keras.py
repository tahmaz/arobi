from keras.models import Sequential
from keras.layers import Dense
import numpy as np

# Input data
x = np.array([[i] for i in range(10)])

# Output data
y = np.array([[1] if i < 5 else [0] for i in range(10)])

# Create the model
model = Sequential()
model.add(Dense(1, input_dim=1, activation='relu'))

# Compile the model
model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])

# Train the model
model.fit(x, y, epochs=1000, batch_size=1)

# Test the model
print(model.predict(x))