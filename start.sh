#!/bin/sh
# Bounding box tracking doesn't work the first time. Need to launch twice.
/root/camera_demo/demo 192.168.1.111 &
sleep 2
pkill demo
sleep 3
/root/camera_demo/demo 192.168.1.111
