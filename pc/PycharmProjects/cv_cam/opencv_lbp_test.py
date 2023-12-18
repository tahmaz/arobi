import cv2
import os

faceCascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")
# video_capture = cv2.VideoCapture(0)

# Call the trained model yml file to recognize faces
recognizer = cv2.face.LBPHFaceRecognizer_create()
recognizer.read("training.yml")

# Names corresponding to each id
names = []
for users in os.listdir("test"):
    names.append(users)

print(names)

count = 0
fail = 0
for name in names:
    for image in os.listdir("test/{}".format(name)):
        path_string = os.path.join("test/{}".format(name), image)
        #path.append(path_string)
        img = cv2.imread(path_string)
        gray_image = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        faces = faceCascade.detectMultiScale(
            gray_image, scaleFactor=1.2, minNeighbors=5, minSize=(100, 100)
        )
        count +=1

        recognize = recognizer.predict(gray_image)
        id = recognize[0]
        per = recognize[1]

        if name != names[id]:
            fail +=1

        #Debug results
        #if id:
        #  print(path_string, name, names[id], per)
        #else:
        #  print(path_string, name, "Unknown" , per)

print("Test images/Recognized: ", count, "/", count-fail )
print("Reslut: ", 100 - (fail/100*count), "% success!" )