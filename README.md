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

Server: run `demo` to start the server. The full options are
```demo [ip] [port] [vchan]```
The default ip is `255.255.255.255` (broadcast). The default port is
4000. The default vchan is 1 which enables the vchan; use 0 to disable
the vchan.

Client: run `java -jar SmaccmViewer.jar`. The full options are
```java -jar SmaccmViewer.jar [port]```
The default port is 4000.


Attacking
=========

The attack assumes the server is running in broadcast mode with port
4000, the default options.

Run `attack.py` from the server side to run the attack demo.

Run `java -jar SmaccmViewer.jar 5000` to see intercepted video.
