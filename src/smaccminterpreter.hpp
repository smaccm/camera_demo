#include "pixyinterpreter.hpp"
#include <netdb.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include <turbojpeg.h>
#include "blobstuff/processblobs.h"
#include "bbox.h"

#ifndef __SMACCMINTERPRETER_HPP__
#define __SMACCMINTERPRETER_HPP__

#define WIDTH 320
#define HEIGHT 200

class SmaccmInterpreter : public PixyInterpreter
{
  public:
    SmaccmInterpreter();
    int connect(char const *ip, int port);

  private:
    struct sockaddr_in clientAddr;
    int socketfd;

    //image stuff
    //r,g,b, each take a byte
    uint8_t processedPixels[WIDTH*HEIGHT*3];
    uint8_t compressedPixels[WIDTH*HEIGHT*3];
    unsigned long compressedLength;
 
    //used for calculating blobs
    ProcessBlobs m_blobs;

    uint16_t width, height;
    uint32_t frame_len;
    uint8_t * pFrame;
    uint32_t cmodelsLen;
    float * cmodels;

    void interpolateBayer(unsigned int width, unsigned int x, unsigned int y, unsigned char *pixel, unsigned int &r, unsigned int &g, unsigned int &b);

    int renderBA81(uint16_t width, uint16_t height, uint8_t *frame, uint8_t * lines);
    int renderBA81(uint16_t width, uint16_t height, uint8_t *frame, uint8_t * lines, uint32_t numBlobs, BlobA * blobs);

    int renderCMV1(uint8_t renderFlags, uint32_t cmodelsLen, float *cmodels, uint16_t width, uint16_t height, uint32_t frameLen, uint8_t *frame); 
    void interpret_data(void * chirp_data[]);

    void compressFrame(void);
};

#endif
