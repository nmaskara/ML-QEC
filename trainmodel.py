from keras.models import Sequential
from keras.layers import Dense, Activation
from keras.callbacks import EarlyStopping
import numpy as np
import matplotlib.pyplot as plt


filename = 'square_4_4_p_20'
sqdata = np.genfromtxt("data/" + filename + ".csv", delimiter=',')
numerrs = np.sum(sqdata[:,0:9], axis=1)
x,y = np.where(sqdata[:,9:13])






insize = 16
model = Sequential()
layer1 = Dense(units=100, activation='relu',input_dim=insize)
layer2 = Dense(units=4, activation='softmax')
model.add(layer1)
model.add(layer2)
model.compile(loss='categorical_crossentropy', optimizer='sgd', metrics=['accuracy'])

early_stopping = EarlyStopping(monitor='val_loss', patience=5)

model.fit(sqdata[:,0:insize], sqdata[:,insize:insize+4], batch_size=32, epochs=400, validation_split=0.3, callbacks=[early_stopping])
model.save("models/" + filename + ".h5")