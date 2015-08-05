#!/usr/bin/env python

from socket import *
from time import sleep
from glob import glob
from os import system
from sys import argv, exit

name = 'skull'
if len(argv) > 1:
    name = argv[1]

# Load images
images = []
for f in sorted(glob('images/%s/*.jpg' % name)):
    with open(f, mode='rb') as f:
        images.append(f.read())
if len(images) == 0:
    print "No images found in images/%s" % name
    exit(0)
            

# Create broadcast socket
s = socket(AF_INET, SOCK_DGRAM)
s.bind(('', 0))
s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

# Send a few flashes        
for i in range(0, 20):
    s.sendto(images[0], ('255.255.255.255', 4000))
    sleep(0.1)

# Kill the demo
# Use SIGINT so it shuts down vchan
system('killall -SIGINT demo')

# Start rogue feed
# Sleep to avoid contention over USB
system('(sleep 3; ./demo 5000) &')

# Send animation on original feed
i = 0
while True:
    s.sendto(images[i], ('255.255.255.255', 4000))
    i = (i + 1) % len(images)
    sleep(0.2)
