#!/bin/bash

#check if process is running
if ps -ef | grep cam | grep -v grep > /dev/null 2>&1
  then
     echo "already running..."
  else
     /usr/bin/screen -d -m -S cam /usr/bin/python3 /root/seon-robot/cam/object_track2.py
fi
