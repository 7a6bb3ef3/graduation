import tensorflow as tf
from keras_preprocessing.image import ImageDataGenerator
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Convolution2D, MaxPooling2D,Activation, Dropout, Flatten, Dense
from tensorflow.keras.optimizers import Adam

# 学习率，太高或太低可能导致无法收敛
learningRate = 0.0015
# 训练中每次迭代步进
stepsPerEpoch = 512
# 迭代次数
epoch = 64
# 防止过拟合参数，过高导致欠拟合
dropoutRate = 0.05
# 随机对训练集横向平移比例
widthShiftRange = 0.02
# 随机对训练集纵向平移比例
heightShiftRange = 0.02,
# 随机XY方向上的放大比例
zoomRange = 0.08
# 随机固定Y对X的平移比例
shearRange = 0.12

stdSize = (40 ,32)


def getClassesDict():
    return ImageDataGenerator().flow_from_directory(directory='./dataset/').class_indices


def train():
    trainDataGen = ImageDataGenerator(
        rotation_range=15,
        shear_range=shearRange,
        zoom_range=zoomRange,
        rescale=1 / 255,
        width_shift_range=widthShiftRange,
        height_shift_range=heightShiftRange,
        fill_mode='nearest'
    )
    trainGenerator = trainDataGen.flow_from_directory(
        directory='./dataset/',
        target_size=stdSize,
        color_mode="grayscale",
        batch_size=10,
    )
    print(trainGenerator.class_indices)

    validDataGen = ImageDataGenerator(
        rotation_range=15,
        shear_range=shearRange,
        zoom_range=zoomRange,
        rescale=1 / 255,
        width_shift_range=widthShiftRange,
        height_shift_range=heightShiftRange,
        fill_mode='nearest'
    )
    validDataGenerator = validDataGen.flow_from_directory(
        directory='./valid/',
        target_size=stdSize,
        color_mode="grayscale",
        batch_size=10,
    )

    model = Sequential()

    model.add(Convolution2D(16, (5, 5), input_shape=(40, 32, 1)))
    model.add(MaxPooling2D(2, 2))
    model.add(Activation('relu'))

    model.add(Convolution2D(32, (5, 5)))
    model.add(MaxPooling2D(2, 2))
    model.add(Activation('relu'))

    model.add(Flatten())

    model.add(Dense(42))
    model.add(Activation('relu'))
    model.add(Dropout(dropoutRate))
    model.add(Dense(42))
    model.add(Activation('sigmoid'))

    adam = Adam(lr=learningRate)
    model.compile(optimizer=adam, loss='categorical_crossentropy', metrics=['accuracy'])
    model.fit(
        trainGenerator,
        steps_per_epoch=stepsPerEpoch,
        epochs=epoch,
        validation_data=validDataGenerator,
    )

    model.save('./model.h5')

    info = 'Graduation Project ShuangJiang Du.\n'
    info += 'Thanks MSRMZNM ANEKI ,ICG ANEKI for help.\n'
    info += 'Using TensorFlow Version' + tf.__version__ + '.\n'
    info += 'Save model as model.h5.\n'
    print(info)
