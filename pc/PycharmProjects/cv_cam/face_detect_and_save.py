from __future__ import print_function
import cv2 as cv
import time

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

    count = 0
    for (x, y, w, h) in faces:
        datetime_now = time.strftime("%Y%m%d-%H%M%S")
        filename = "./images/detected/" + datetime_now + "_" + str(count) + '.jpg'
        face = frame[y:y + h, x:x + w]  # slice the face from the image
        cv.imwrite(filename, face)  # save the image
        count += 1
        cv.rectangle(frame, (x, y), (x + w, y + h), (255, 0, 0), 2)
        #print("face center is:", x + w / 2, y + h / 2)

    cv.imshow(window_capture_name, frame)


    key = cv.waitKey(30)
    if key == ord('q') or key == 27:
        break