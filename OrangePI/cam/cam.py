import cv2
import numpy

cap = cv2.VideoCapture(1)  #ignore the errors
#cap.set(3, 960)        #Set the width important because the default will timeout
                       #ignore the error or false response
#cap.set(4, 544)        #Set the height ignore the errors
r, frame = cap.read()
cv2.imwrite("photo.jpg", frame)
