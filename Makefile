# This make file was created by John backes to
# make the libpixyusb build more portable for the
# odroid

CC=g++
HEADER_INCLUDES=-I /usr/include/libusb-1.0
LIB_INCLUDES=-L/usr/lib/x86_64-linux-gnu
CFLAGS=$(HEADER_INCLUDES) -D__LIBPIXY_VERSION__=\"0.3\" -D__LINUX__ -g3 -c
LIBS=$(LIB_INCLUDES) -lusb-1.0 -lboost_chrono -lboost_system -lboost_thread

all: demo

demo: chirp.o chirpreceiver.o hello_pixy.o pixy.o pixyinterpreter.o usblink.o timer.o
	$(CC) chirp.o chirpreceiver.o hello_pixy.o timer.o\
          pixy.o pixyinterpreter.o usblink.o $(LIBS) -o demo

pixy-lib: hello_pixy.o
	$(CC) hello_pixy.o -L/home/rockwell/pixy/build/libpixyusb/ -lpixyusb $(LIBS) -o pixy-lib
	
chirp.o:
	$(CC) $(CFLAGS) chirp.cpp

chirpreceiver.o:
	$(CC) $(CFLAGS) chirpreceiver.cpp

hello_pixy.o:
	$(CC) $(CFLAGS) hello_pixy.cpp

pixy.o:
	$(CC) $(CFLAGS) pixy.cpp

pixyinterpreter.o:
	$(CC) $(CFLAGS) pixyinterpreter.cpp

usblink.o:
	$(CC) $(CFLAGS) usblink.cpp
timer.o:
	$(CC) $(CFLAGS) utils/timer.cpp
clean:
	rm -f *.o; \
	rm -f demo
