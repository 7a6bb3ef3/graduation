from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing import image
import numpy as np
import os
import snnn

model = load_model('./model.h5')

predictDir = './predict'
files = os.listdir(predictDir)
classDict = {v: k for k, v in snnn.getClassesDict().items()}

info = '\n\nGraduation Project ShuangJiang Du.\n'
info += 'Thanks MSRMZNM ANEKI ,ICG ANEKI for help.\n'
print(info)
charSeq = ''
for file in files:
    img = image.load_img(os.path.join(predictDir ,file) ,target_size=snnn.stdSize ,color_mode='grayscale')
    imgArr3 = image.img_to_array(img)
    imgArr4 = np.expand_dims(imgArr3 ,axis=0)
    imgArr4 = imgArr4.astype(float) / float(255)
    re = model.predict_classes(imgArr4)
    charSeq += classDict[re[0]]
    print(classDict[re[0]] +  ' -> ' + file)
print('\n' + charSeq + '\n')



