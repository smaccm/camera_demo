# This make file was created by John backes to
# make the libpixyusb build more portable for the
# odroid

CC=gcc
CXX=g++
HEADER_INCLUDES=-I /usr/include/libusb-1.0
LIB_INCLUDES=-L/usr/lib/x86_64-linux-gnu
CFLAGS=$(HEADER_INCLUDES) -D__LIBPIXY_VERSION__=\"0.3\" -D__LINUX__ -g3 -c
LIBS=$(LIB_INCLUDES) -lusb-1.0 -lboost_chrono -lboost_system -lboost_thread -pthread -lturbojpeg

all: demo

demo: chirp.o chirpreceiver.o demo.o pixy.o pixyinterpreter.o smaccminterpreter.o usblink.o timer.o blob.o blobs.o colorlut.o processblobs.o qqueue.o bbox.o
	$(CXX) $^ $(LIBS) -o demo

smaccminterpreter.o: smaccminterpreter.cpp
	$(CXX) $(CFLAGS) smaccminterpreter.cpp

chirp.o: chirp.cpp
	$(CXX) $(CFLAGS) chirp.cpp

chirpreceiver.o: chirpreceiver.cpp
	$(CXX) $(CFLAGS) chirpreceiver.cpp

demo.o: demo.cpp
	$(CXX) $(CFLAGS) demo.cpp

pixy.o: pixy.cpp
	$(CXX) $(CFLAGS) pixy.cpp

pixyinterpreter.o: pixyinterpreter.cpp
	$(CXX) $(CFLAGS) pixyinterpreter.cpp

usblink.o: usblink.cpp
	$(CXX) $(CFLAGS) usblink.cpp

timer.o: utils/timer.cpp
	$(CXX) $(CFLAGS) utils/timer.cpp

blob.o: blobstuff/blob.cpp
	$(CXX) $(CFLAGS) blobstuff/blob.cpp

blobs.o: blobstuff/blobs.cpp
	$(CXX) $(CFLAGS) blobstuff/blobs.cpp

colorlut.o: blobstuff/colorlut.cpp
	$(CXX) $(CFLAGS) blobstuff/colorlut.cpp

processblobs.o: blobstuff/processblobs.cpp
	$(CXX) $(CFLAGS) blobstuff/processblobs.cpp

qqueue.o: blobstuff/qqueue.cpp
	$(CXX) $(CFLAGS) blobstuff/qqueue.cpp

bbox.o: bbox.c
	$(CXX) $(CFLAGS) bbox.c

clean:
	rm -f *.o demo
