#!/usr/bin/env python
#coding: utf8
  
import lirc
import time
import os
  
sockid=lirc.init("myprogram")
  
while True:
    codeIR = lirc.nextcode()
    #print(codeIR)
    if codeIR != []:
        if codeIR[0] == "UP":
            f = open("/root/seon-robot/action/motor/command.txt", "w")
            f.write("8.100.000.19.19.255.0.8")
            f.close()
        if codeIR[0] == "DOWN":
            f = open("/root/seon-robot/action/motor/command.txt", "w")
            f.write("8.100.000.29.29.255.0.8")
            f.close()
        if codeIR[0] == "RIGHT":
            f = open("/root/seon-robot/action/motor/command.txt", "w")
            f.write("8.100.000.27.17.255.0.8")
            f.close()
        if codeIR[0] == "LEFT":
            f = open("/root/seon-robot/action/motor/command.txt", "w")
            f.write("8.100.000.17.27.255.0.8")
            f.close()
        if codeIR[0] == "OK":
            f = open("/root/seon-robot/action/motor/command.txt", "w")
            f.write("8.100.000.00.00.255.0.8")
            f.close()
        if codeIR[0] == "1":
            f = open("/root/seon-robot/action/motor/command.txt", "w")
            f.write("8.050.000.00.00.255.0.8")
            f.close()
        if codeIR[0] == "2":
            f = open("/root/seon-robot/action/motor/command.txt", "w")
            f.write("8.130.000.00.00.255.0.8")
            f.close()
