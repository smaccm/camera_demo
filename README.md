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

Run `demo` to start the server. The default port is 4000 and can be
set using the first command line argument.

To connect, run `java -jar SmaccmViewer.jar <hostname> [port]`. The
default port is 4000.

The server must be started before the client connects. The server only
services the first client. To reconnect the client, the server must be
restarted.


Attacking
=========

Run `attack.sh` from the server side to run the attack demo.
