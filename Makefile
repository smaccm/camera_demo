# This make file was created by John backes to
# make the libpixyusb build more portable for the
# odroid

CC=g++
HEADER_INCLUDES=-I /usr/include/libusb-1.0
LIB_INCLUDES=-L/usr/lib/x86_64-linux-gnu
CFLAGS=$(HEADER_INCLUDES) -D__LIBPIXY_VERSION__=\"0.3\" -D__LINUX__ -g3 -c
LIBS=$(LIB_INCLUDES) -lusb-1.0 -lboost_chrono -lboost_system -lboost_thread

all: demo

demo: chirp.o chirpreceiver.o demo.o pixy.o pixyinterpreter.o usblink.o timer.o
	$(CC) chirp.o chirpreceiver.o demo.o timer.o\
          pixy.o pixyinterpreter.o usblink.o $(LIBS) -o demo

chirp.o: chirp.cpp
	$(CC) $(CFLAGS) chirp.cpp

chirpreceiver.o: chirpreceiver.cpp
	$(CC) $(CFLAGS) chirpreceiver.cpp

demo.o: demo.cpp
	$(CC) $(CFLAGS) demo.cpp

pixy.o: pixy.cpp
	$(CC) $(CFLAGS) pixy.cpp

pixyinterpreter.o: pixyinterpreter.cpp
	$(CC) $(CFLAGS) pixyinterpreter.cpp

usblink.o: usblink.cpp
	$(CC) $(CFLAGS) usblink.cpp

timer.o: utils/timer.cpp
	$(CC) $(CFLAGS) utils/timer.cpp

clean:
	rm -f *.o; \
	rm -f demo
