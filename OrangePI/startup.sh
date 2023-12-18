mkdir /mnt/ramdisk/action
cp -fr /root/seon-robot/action/* /mnt/ramdisk/action/

/usr/bin/screen -d -m -S oled /usr/bin/python2 /root/seon-robot/OrangePi-OLED/examples/test4.py
/usr/bin/screen -d -m -S sensor /usr/bin/python3 /root/seon-robot/orangepi_PC_gpio_pyH3/examples/sensor_test2.py
/usr/bin/screen -d -m -S motor /usr/bin/python2 /root/seon-robot/OrangePi-OLED/examples/motor_command.py
/usr/bin/screen -d -m -S ir /usr/bin/python2 /root/seon-robot/ir/ir.py
#/usr/bin/screen -d -m -S cam /usr/bin/python3 /root/seon-robot/cam/object_track2.py
/usr/bin/screen -d -m -S cam /usr/bin/python3 /root/seon-robot/cam/detect_face_and_turn2.py