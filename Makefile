# This make file was created by John backes to
# make the libpixyusb build more portable for the
# odroid

CC=g++
HEADER_INCLUDES=-I /usr/include/libusb-1.0
LIB_INCLUDES=-L/usr/lib/x86_64-linux-gnu
CFLAGS=$(HEADER_INCLUDES) -D__LIBPIXY_VERSION__=\"0.3\" -D__LINUX__ -g3 -c
LIBS=$(LIB_INCLUDES) -lusb-1.0 -lboost_chrono -lboost_system -lboost_thread -pthread -lz

all: demo

vchan_test: vchan_blob_test.o vchan_blob_interface.o sel4libvchan.o
	$(CC) vchan_blob_test.o vchan_blob_interface.o sel4libvchan.o -o vchan_test

demo: chirp.o chirpreceiver.o demo.o pixy.o pixyinterpreter.o smaccminterpreter.o usblink.o timer.o blob.o blobs.o colorlut.o processblobs.o qqueue.o sel4libvchan.o vchan_blob_interface.o
	$(CC) chirp.o chirpreceiver.o demo.o timer.o \
          pixy.o pixyinterpreter.o smaccminterpreter.o \
          usblink.o blob.o blobs.o colorlut.o processblobs.o \
          qqueue.o sel4libvchan.o vchan_blob_interface.o $(LIBS) -o demo

smaccminterpreter.o: smaccminterpreter.cpp
	$(CC) $(CFLAGS) smaccminterpreter.cpp

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

blob.o: blobstuff/blob.cpp
	$(CC) $(CFLAGS) blobstuff/blob.cpp

blobs.o: blobstuff/blobs.cpp
	$(CC) $(CFLAGS) blobstuff/blobs.cpp

colorlut.o: blobstuff/colorlut.cpp
	$(CC) $(CFLAGS) blobstuff/colorlut.cpp

processblobs.o: blobstuff/processblobs.cpp
	$(CC) $(CFLAGS) blobstuff/processblobs.cpp

qqueue.o: blobstuff/qqueue.cpp
	$(CC) $(CFLAGS) blobstuff/qqueue.cpp

vchan_blob_interface.o: vchan_code/vchan_blob_interface.c
	gcc $(CFLAGS) vchan_code/vchan_blob_interface.c

sel4libvchan.o: vchan_code/sel4libvchan.c
	gcc $(CFLAGS) vchan_code/sel4libvchan.c

vchan_blob_test.o: vchan_code/vchan_blob_test.c
	$(CC) $(CFLAGS) vchan_code/vchan_blob_test.c

clean:
	rm -f *.o; rm -f demo; rm -f vchan_test;
