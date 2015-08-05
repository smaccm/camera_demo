Camera Demo
===========

Software for the SMACCM Phase 2 Camera Demo.


Build Instructions
==================

Install dependencies:

```
sudo apt-get install libboost-chrono-dev \
                     libboost-system-dev \
                     libboost-thread-dev \
                     libusb-1.0-0-dev \
                     libjpeg-turbo8-dev
```

Run `make`.


Running
=======

Server: run `demo` to start the server. The default port is 4000 and can be
set using the first command line argument.

Client: run `java -jar SmaccmViewer.jar [port]`. The default port is
4000 and can be set using the first command line argument.

Attacking
=========

Run `attack.py` from the server side to run the attack demo.

Run `java -jar SmaccmViewer.jar 5000` to see intercepted video.
