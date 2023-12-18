#!/usr/bin/env python
#coding: utf8
  
import lirc
import time
import os
  
sockid=lirc.init("myprogram")
  
while True:
    codeIR = lirc.nextcode()
    print(codeIR)
    if codeIR != []:
        if codeIR[0] == "UP":
            print("UP")
  
        elif codeIR[0] == "DOWN":
            print("DOWN")
        else:
            print("Cannot find")
