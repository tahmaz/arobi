import cv2,time, random

camera = cv2.VideoCapture(1)  # веб камера
rand_num = 0

while True:
    iSee = False     # флаг: был ли найден контур
    controlX = 0.0      # нормализованное отклонение цветного объекта от центра кадра в диапазоне [-1; 1]

    centerx = 640/2
    centery = 360/2	
    imgcx = centerx
    imgcy = centery
    ferq = 50
    
    success, frame = camera.read()  # читаем кадр с камеры
    
    if success:     # если прочитали успешно
        height, width = frame.shape[0:2]    # получаем разрешение кадра
            
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)    # переводим кадр из RGB в HSV
        #binary = cv2.inRange(hsv, (18, 60, 100), (32, 255, 255))  # пороговая обработка кадра (выделяем все желтое)
        binary = cv2.inRange(hsv, (30, 115, 0), (45, 255, 255))  # пороговая обработка кадра (выделяем все желтое)
        #binary = cv2.inRange(hsv, (0, 0, 0), (255, 255, 35))  # пороговая обработка кадра (выделяем все черное)

        """
        # Чтобы выделить все красное необходимо произвести две пороговые обработки, т.к. тон красного цвета в hsv 
        # находится в начале и конце диапазона hue: [0; 180), а в openCV, хз почему, этот диапазон не закольцован.
        # поэтому выделяем красный цвет с одного и другого конца, а потом просто складываем обе битовые маски вместе
        
        bin1 = cv2.inRange(hsv, (0, 60, 70), (10, 255, 255)) # красный цвет с одного конца
        bin2 = cv2.inRange(hsv, (160, 60, 70), (179, 255, 255)) # красный цвет с другого конца
        binary = bin1 + bin2  # складываем битовые маски
        """
        #roi = cv2.bitwise_and(frame, frame, mask=binary)    # за счет полученной маски можно выделить найденный объект из общего кадра

        contours, _ = cv2.findContours(binary, cv2.RETR_EXTERNAL,
                                       cv2.CHAIN_APPROX_NONE)  # получаем контуры выделенных областей 

        if len(contours) != 0:  # если найден хоть один контур
            maxc = max(contours, key=cv2.contourArea)  # находим наибольший контур
            moments = cv2.moments(maxc) # получаем моменты этого контура
            rand_num = random.randrange(0, 2)
            """
            # moments["m00"] - нулевой момент соответствует площади контура в пикселях,
            # поэтому, если в битовой маске присутствуют шумы, можно вместо
            # if moments["m00"] != 0:  # использовать
                
            if moments["m00"] > 20: # тогда контуры с площадью меньше 20 пикселей не будут учитываться 
            """
            if moments["m00"] > 20: #  контуры с площадью меньше 20 пикселей не будут учитываться 
                imgcx = int(moments["m10"] / moments["m00"])  # находим координаты центра контура (найденного объекта) по x
                imgcy = int(moments["m01"] / moments["m00"])  # находим координаты центра контура (найденного объекта) по y

                #print(cx,cy)
                f = open("/mnt/ramdisk/action/motor/command.txt", "w")
                if centery - imgcy > ferq :
                    f.write("8.205.030.000.000")
                    print("UP")
                elif centery - imgcy < -ferq :
                    f.write("8.305.030.000.000")
                    print("Down")
                elif centerx - imgcx > ferq :
                    happy_screen = 1
                    if(rand_num == 1):
                        f.write("8.400.030.171.071")
                    else:
                        f.write("8.400.030.071.271")
                    print("Left")
                elif centerx - imgcx < -ferq :
                    happy_screen = 1
                    if(rand_num == 1):
                        f.write("8.400.030.071.171")
                    else:
                        f.write("8.400.030.271.071")
                    print("Right")
                else:
                    f.write("8.400.030.171.171")
                    print("Forward")
                f.close()


        
    if cv2.waitKey(1) == ord('q'):  # чтоб выйти надо нажать 'q' на клавиатуре
        break

camera.release()
cv2.destroyAllWindows()
