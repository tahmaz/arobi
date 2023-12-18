import cv2, time, random

camera = cv2.VideoCapture(1)  # веб камера

lps = 0
lps_last_time = 0
lps_print_sec = 10

centerx = 640/2
centery = 360/2	
imgcx = centerx
imgcy = centery
imgcx_new = centerx
imgcy_new = centery

rand_num = 0
happy_screen = 0


while True:
    ferq = 50
    
    current_millis = round(time.time() * 1000)
    
    success, frame = camera.read()  # читаем кадр с камеры
    
    if success:     # если прочитали успешно
    
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
        
        if(len(faces) > 0):
            #print("Found {0} Faces!".format(len(faces)))
            count = 0
            for (x, y, w, h) in faces:
                imgcx_new = x+w/2
                imgcy_new = y+h/2
                #generate random 0 or 1
                rand_num = random.randrange(0, 2)
                #print("x={0}, y={1}, imgcx={2}, imgcy={3}, imgcx_new={4}, imgcy_new={5}".format(x,y,imgcx,imgcy,imgcx_new,imgcy_new))
                #print("Rand_num = {0}".format(rand_num))
                
                datetime_now = time.strftime("%Y%m%d-%H%M%S")
                filename = "/root/seon-robot/cam/images/detected/" + datetime_now + "_" + str(count) + '.jpg'
                face = frame[y:y + h, x:x + w]  # slice the face from the image
                count += 1
                
                #give command for motors
                f = open("/mnt/ramdisk/action/motor/command.txt", "w")
                if (centery - imgcy_new > ferq)  and abs(imgcy - imgcy_new) > 20 :
                    imgcy = imgcy_new
                    happy_screen = 1
                    f.write("8.205.030.000.000")
                    cv2.imwrite(filename, face)  # save the image
                    print("UP")
                elif (centery - imgcy_new < -ferq ) and abs(imgcy - imgcy_new) > 20 :
                    imgcy = imgcy_new
                    happy_screen = 1
                    f.write("8.305.030.000.000")
                    cv2.imwrite(filename, face)  # save the image
                    print("Down")
                elif (centerx - imgcx_new > ferq)  and abs(imgcx - imgcx_new) > 20 :
                    imgcx = imgcx_new
                    happy_screen = 1
                    if(rand_num == 1):
                        f.write("8.400.030.171.071")
                    else:
                        f.write("8.400.030.071.271")
                    cv2.imwrite(filename, face)  # save the image                        
                    print("Left")
                elif (centerx - imgcx_new < -ferq ) and abs(imgcx - imgcx_new) > 20 :
                    imgcx = imgcx_new
                    happy_screen = 1
                    if(rand_num == 1):
                        f.write("8.400.030.071.171")
                    else:
                        f.write("8.400.030.271.071")
                    cv2.imwrite(filename, face)  # save the image
                    print("Right")
                #else:
                #   f.write("8.400.030.173.173")
                #   print("Forward")
                f.close()
            
            #give command for screen
            if(happy_screen == 1 ):
                f = open("/mnt/ramdisk/action/oled/command.txt", "w")
                f.write("happy")
                f.close()
                happy_screen = 0

    #calc lps
    if ((current_millis - lps_last_time) > (lps_print_sec * 1000)):
        lps_last_time = round(time.time() * 1000)
        print("LPS: {0}".format(lps))
        lps = 0

    lps += 1