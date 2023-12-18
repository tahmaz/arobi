import cv2
import time

#cap.set(3, 640)        #Set the width important because the default will timeout
#cap.set(4, 360)        #Set the height ignore the errors
centerx = 640/2
centery = 360/2	
imgcx = centerx
imgcy = centery
ferq = 20


def capture():
    cap = cv2.VideoCapture(1)
    r, frame = cap.read()
    #cv2.imwrite("photo.jpg", frame)
    
    # Load the cascade
    face_cascade = cv2.CascadeClassifier('/root/seon-robot/cam/haarcascade_frontalface_default.xml')
    # Read the input image
    #img = cv2.imread('photo.jpg')
    img = frame
    # Convert into grayscale
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    faces = face_cascade.detectMultiScale(
            gray,
            scaleFactor=1.3,
            minNeighbors=3,
            minSize=(30, 30)
    )
    
    print("Found {0} Faces!".format(len(faces)))
    global imgcx
    global imgcy
    if(len(faces) > 0):
        for (x, y, w, h) in faces:
            imgcx = x+w/2
            imgcy = y+h/2
    else:
        imgcx = centerx
        imgcy = centery


while True:
    capture()

    if centerx - imgcx > ferq :
        f = open("/root/seon-robot/action/motor/command.txt", "w")
        f.write("8.400.400.11.21.255.0.8")
        f.close()
        time.sleep(0.3)
        print("face center is:",imgcx, imgcy )
    elif centerx - imgcx < -ferq :
        f = open("/root/seon-robot/action/motor/command.txt", "w")
        f.write("8.400.400.21.11.255.0.8")
        f.close()
        time.sleep(0.3)

    if centery - imgcy > ferq :
        f = open("/root/seon-robot/action/motor/command.txt", "w")
        f.write("8.205.400.00.00.255.0.8")
        f.close()
        time.sleep(0.3)
    elif centery - imgcy < -ferq :
        f = open("/root/seon-robot/action/motor/command.txt", "w")
        f.write("8.305.400.00.00.255.0.8")
        f.close()
        time.sleep(0.3)


