from __future__ import print_function
import cv2 as cv

centerx = 640/2
centery = 360/2
ferq = 20

window_capture_name = 'Video Capture'

cap = cv.VideoCapture(0)
cv.namedWindow(window_capture_name)

while True:

    ret, frame = cap.read()
    if frame is None:
        break

    # Load the cascade
    face_cascade = cv.CascadeClassifier('haarcascade_frontalface_default.xml')
    # Convert into grayscale
    gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)

    faces = face_cascade.detectMultiScale(
        gray,
        scaleFactor=1.5,
        minNeighbors=3,
        minSize=(30, 30)
    )
    #print("Found {0} Faces!".format(len(faces)))

    # Detect faces
    # faces = face_cascade.detectMultiScale(gray, 1.1, 4)
    # Draw rectangle around the faces
    for (x, y, w, h) in faces:
        cv.rectangle(frame, (x, y), (x + w, y + h), (255, 0, 0), 2)
        #print("face center is:", x + w / 2, y + h / 2)

    cv.imshow(window_capture_name, frame)

    if (len(faces) > 0):
        if centerx - x > ferq :
            print("left")
        elif centerx - x < -ferq :
            print("rigt")

        if centery - y > ferq :
            print("up")
        elif centery - y < -ferq :
            print("down")


    key = cv.waitKey(30)
    if key == ord('q') or key == 27:
        break