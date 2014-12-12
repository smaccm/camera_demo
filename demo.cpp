//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include <boost/asio.hpp>
#include "pixy.h"
#include "pixyinterpreter.hpp"
#include "bitmap_image.hpp"

using boost::asio::ip::tcp;

#define BLOCK_BUFFER_SIZE    25

// Pixy Block buffer // 
struct Block blocks[BLOCK_BUFFER_SIZE];
extern PixyInterpreter interpreter;
void handle_SIGINT(int unused)
{
  // On CTRL+C - abort! //

  printf("\nBye!\n");
  exit(0);
}

inline void interpolateBayer(unsigned int width, unsigned int x, unsigned int y, unsigned char *pixel, unsigned int &r, unsigned int &g, unsigned int &b)
{
    if (y&1)
    {
        if (x&1)
        {
            r = *pixel;
            g = (*(pixel-1)+*(pixel+1)+*(pixel+width)+*(pixel-width))>>2;
            b = (*(pixel-width-1)+*(pixel-width+1)+*(pixel+width-1)+*(pixel+width+1))>>2;
        }
        else
        {
            r = (*(pixel-1)+*(pixel+1))>>1;
            g = *pixel;
            b = (*(pixel-width)+*(pixel+width))>>1;
        }
    }
    else
    {
        if (x&1)
        {
            r = (*(pixel-width)+*(pixel+width))>>1;
            g = *pixel;
            b = (*(pixel-1)+*(pixel+1))>>1;
        }
        else
        {
            r = (*(pixel-width-1)+*(pixel-width+1)+*(pixel+width-1)+*(pixel+width+1))>>2;
            g = (*(pixel-1)+*(pixel+1)+*(pixel+width)+*(pixel-width))>>2;
            b = *pixel;
        }
    }
}

int renderBA81(uint16_t width, uint16_t height, uint8_t *frame, uint32_t ** lines, bitmap_image &image)
{
    uint16_t x, y;
    uint32_t *line;
    uint32_t r, g, b;

    image_drawer drawer(image);
    // skip first line
    frame += width;

    // don't render top and bottom rows, and left and rightmost columns because of color
    // interpolation
    //QImage img(width-2, height-2, QImage::Format_RGB32);

    for (y=1; y<height-1; y++)
    {
        line = (unsigned int *)lines[y-1];
        frame++;
        for (x=1; x<width-1; x++, frame++)
        {
            interpolateBayer(width, x, y, frame, r, g, b);
            *line++ = (0x40<<24) | (r<<16) | (g<<8) | (b<<0);
            drawer.pen_color((char)r,(char)g,(char)b);            
            drawer.plot_pixel(x,y);
        }
        frame++;
    }

    return 0;
}

int main(int argc, char * argv[])
{
  int      index;
  int      blocks_copied;
  int      pixy_init_status;

  if(argc != 2){
    std::cerr << "Usage: demo <port>" << std::endl;
      return 1;
  }

  // Catch CTRL+C (SIGINT) signals //
  signal(SIGINT, handle_SIGINT);

  printf("Hello Pixy:\n libpixyusb Version: %s\n", __LIBPIXY_VERSION__);

  //setup the socket for sending pictures
  boost::asio::io_service io_service;
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), atoi(argv[1])));

  int32_t response;
  uint32_t fourcc;
  int8_t renderflags;
  uint16_t width, height, sentWidth, sentHeight;
  uint32_t numPixels;

  sentWidth = 320;
  sentHeight = 200;
  uint8_t * pixels = (uint8_t *)malloc(sentWidth*sentHeight*sizeof(uint8_t));

  uint32_t ** lines = (uint32_t **)malloc(sentHeight*sizeof(uint32_t *));
  
  int n;
  for(n = 0; n < sentHeight; n++){
    lines[n] = (uint32_t *)malloc(sentWidth*sizeof(uint32_t));
  }

  bitmap_image image(sentWidth, sentHeight);
  
  // Connect to Pixy //
  pixy_init_status = pixy_init();

  // Was there an error initializing pixy? //
  if(!pixy_init_status == 0)
  {
    // Error initializing Pixy //
    printf("pixy_init(): ");
    pixy_error(pixy_init_status);

    return pixy_init_status;
  }

  for(;;){

    int return_value = pixy_command("cam_getFrame", // String id for remote procedure
      0x01, 0x21, // mode
      0x02, 0, // X-offset
      0x02, 0, // Y-offset
      0x02, 320, // width
      0x02, 200, // height
      END, // separator
      &response, // pointer to the memory address for return value
      &fourcc,
      &renderflags,
      &width,
      &height,
      &numPixels,
      &pixels, // pointer to memory address for returned frame
      END);

    printf("return value: %d\n", return_value);
    printf("response value :%d\n", response);
    printf("num pixels: %d\n", numPixels);
    
    renderBA81(width, height, pixels, lines, image);

//    try{
//      tcp::socket socket(io_service);
//      acceptor.accept(socket);
//      std::string message = "hello client\n";
//    
//      boost::system::error_code ignored_error;
//      boost::asio::write(socket, boost::asio::buffer(message),
//        boost::asio::transfer_all(), ignored_error);
//    }
//    catch (std::exception& e)
//    {
//      std::cerr << e.what() << std::endl;
//    }

    image.save_image("output.bmp");
    usleep(1000000);
     
  }
     
}  
   
   
   
   
   
   
   
   
