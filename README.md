Camera Demo
===========

Software for the SMACCM Camera Demo.


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

The Pixy cam needs a special firmware - upload [firmware-1.0.2beta.hex](https://github.com/smaccm/camera_demo/blob/master/notes/firmware-1.0.2beta.hex) using [PixyMon](https://github.com/charmedlabs/pixy) app.

Running
=======

Server: run `demo` to start the server. The full options are
```demo [ip] [port]```
The default ip is `255.255.255.255` (broadcast). The default port is
4000.

Client: run `java -jar SmaccmViewer.jar`. The full options are
```java -jar SmaccmViewer.jar [port]```
The default port is 4000.


Attacking
=========

The attack assumes the server is running in broadcast mode with port
4000, the default options.

Run `attack.py` from the server side to run the attack demo.

Run `java -jar SmaccmViewer.jar 5000` to see intercepted video.
