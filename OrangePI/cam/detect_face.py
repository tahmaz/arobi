import cv2

cap = cv2.VideoCapture(1)  #ignore the errors
#cap.set(3, 640)        #Set the width important because the default will timeout
#cap.set(4, 360)        #Set the height ignore the errors
r, frame = cap.read()
cv2.imwrite("photo.jpg", frame)

# Load the cascade
face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')
# Read the input image
img = cv2.imread('photo.jpg')
# Convert into grayscale
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

faces = face_cascade.detectMultiScale(
        gray,
        scaleFactor=1.3,
        minNeighbors=3,
        minSize=(30, 30)
)

print("Found {0} Faces!".format(len(faces)))

# Detect faces
#faces = face_cascade.detectMultiScale(gray, 1.1, 4)
# Draw rectangle around the faces
for (x, y, w, h) in faces:
    cv2.rectangle(img, (x, y), (x+w, y+h), (255, 0, 0), 2)
    print("face center is:", x+w/2, y+h/2 )

cv2.imwrite("photo2.jpg", img)

