#include "pixyinterpreter.hpp"
#include <boost/asio.hpp>
#include  <boost/thread/thread.hpp>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include "blobstuff/processblobs.h"
#include "vchan_code/blob_interface.h"

#ifndef __SMACCMINTERPRETER_HPP__
#define __SMACCMINTERPRETER_HPP__

#define SERVICE_PORT 4000
using boost::asio::ip::tcp;

class SmaccmInterpreter : public PixyInterpreter
{
  public:
    SmaccmInterpreter();
    int connect();
    int connect(int port);
  private:
    //socket stuff
    //boost::asio::io_service io_service;
    //tcp::acceptor acceptor;
    //tcp::socket socket;
    struct sockaddr_in myaddr; //our address
    struct sockaddr_in remaddr; //remote address
    int recvfd; //receive file descriptor
    int sendfd; //send file descritptor

    //image stuff
    static const int sentWidth = 320;
    static const int sentHeight = 200;
    //r,g,b, each take a byte
    uint8_t processedPixels[sentWidth*sentHeight*3+1];//extra byte is a hack to keep track of udp message number
 
    //used for calculating blobs
    ProcessBlobs m_blobs;

    //mutex for sending images
    boost::mutex imageMutex;  

    //global vars for receiving new frame
    int fNewFrame;

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

    void sendFrame(void);
    void waitForResponse(void);
};

#endif
