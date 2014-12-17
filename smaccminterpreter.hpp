#include "pixyinterpreter.hpp"
#include <boost/asio.hpp>
#include  <boost/thread/thread.hpp>
#include "bitmap_image.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <iostream>

#ifndef __SMACCMINTERPRETER_HPP__
#define __SMACCMINTERPRETER_HPP__

using boost::asio::ip::udp;

class SmaccmInterpreter : public PixyInterpreter
{
  public:
    SmaccmInterpreter();
    int init();
  private:
    //socket stuff
    udp::endpoint remote_endpoint;
    boost::asio::io_service io_service;
    udp::socket socket;

    //image stuff
    static const int sentWidth = 320;
    static const int sentHeight = 200;
    uint32_t processedPixels[sentWidth*sentHeight];
 
    boost::mutex imageMutex;  
    bitmap_image * pImage;

    void interpolateBayer(unsigned int width, unsigned int x, unsigned int y, unsigned char *pixel, unsigned int &r, unsigned int &g, unsigned int &b);

    int renderBA81(uint16_t width, uint16_t height, uint8_t *frame, uint32_t * lines, bitmap_image * pImage);

    void interpret_data(void * chirp_data[]);

    void sendFrame(void);
};

#endif
