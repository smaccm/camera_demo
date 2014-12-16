#include "pixyinterpreter.hpp"
#include <boost/asio.hpp>
#include "bitmap_image.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <iostream>

#ifndef __SMACCMINTERPRETER_HPP__
#define __SMACCMINTERPRETER_HPP__

using boost::asio::ip::tcp;

class SmaccmInterpreter : public PixyInterpreter
{
  public:
    SmaccmInterpreter();
    int init();
  private:
    //socket stuff
    //boost::asio::io_service io_service;
    //tcp::acceptor acceptor;
    //tcp::socket socket;

    static const int sentWidth = 320;
    static const int sentHeight = 200;
    uint8_t pixels[sentWidth*sentHeight];
    uint32_t processedPixels[sentWidth*sentHeight];
    

    void interpolateBayer(unsigned int width, unsigned int x, unsigned int y, unsigned char *pixel, unsigned int &r, unsigned int &g, unsigned int &b);

    int renderBA81(uint16_t width, uint16_t height, uint8_t *frame, uint32_t * lines, bitmap_image &image);

};

#endif
