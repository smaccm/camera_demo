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
#include "pixy.h"
#include "pixyinterpreter.hpp"
#include "bitmap_image.hpp"

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

int renderBA81(uint16_t width, uint16_t height, uint8_t *frame, uint32_t ** lines, image_drawer * drawer)
{
    uint16_t x, y;
    uint32_t *line;
    uint32_t r, g, b;

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
            drawer->pen_color((char)r,(char)g,(char)b);            
            drawer->plot_pixel(x,y);
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

  // Catch CTRL+C (SIGINT) signals //
  signal(SIGINT, handle_SIGINT);

  printf("Hello Pixy:\n libpixyusb Version: %s\n", __LIBPIXY_VERSION__);

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

  // Request Pixy firmware version //
  {
    uint16_t major;
    uint16_t minor;
    uint16_t build;
    int      return_value;

    return_value = pixy_get_firmware_version(&major, &minor, &build);

    if (return_value) {
      // Error //
      printf("Failed to retrieve Pixy firmware version. ");
      pixy_error(return_value);

      return return_value;
    } else {
      // Success //
      printf(" Pixy Firmware Version: %d.%d.%d\n", major, minor, build);
    }
  }

  // Pixy Command Examples //
  {
    int32_t response;
    int     return_value;

    // Execute remote procedure call "cam_setAWB" with one output (host->pixy) parameter (Value = 1)
    //
    //   Parameters:                 Notes:
    //
    //   pixy_command("cam_setAWB",  String identifier for remote procedure
    //                        0x01,  Length (in bytes) of first output parameter
    //                           1,  Value of first output parameter
    //                           0,  Parameter list seperator token (See value of: END_OUT_ARGS)
    //                   &response,  Pointer to memory address for return value from remote procedure call
    //                           0); Parameter list seperator token (See value of: END_IN_ARGS)
    //

    // Enable auto white balance //
    return_value = pixy_command("cam_setAWB", 0x01, 1, 0, &response, 0);

    // Execute remote procedure call "cam_getAWB" with no output (host->pixy) parameters
    //
    //   Parameters:                 Notes:
    //
    //   pixy_command("cam_setAWB",  String identifier for remote procedure
    //                           0,  Parameter list seperator token (See value of: END_OUT_ARGS)
    //                   &response,  Pointer to memory address for return value from remote procedure call
    //                           0); Parameter list seperator token (See value of: END_IN_ARGS)
    //

    // Get auto white balance //
    return_value = pixy_command("cam_getAWB", 0, &response, 0);

    // Set auto white balance back to disabled //
    pixy_command("cam_setAWB", 0x01, 0, 0, &response, 0);
  }

  int32_t response;
  uint32_t fourcc;
  int8_t renderflags;
  uint16_t width, height;
  uint32_t numPixels;
  uint8_t * pixels = (uint8_t *)malloc(320*200*sizeof(uint8_t));

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

  uint32_t ** lines = (uint32_t **)malloc(200*sizeof(uint32_t *));
  int n;
  
  for(n = 0; n < 200; n++){
    lines[n] = (uint32_t *)malloc(320*sizeof(uint32_t));
  }

  bitmap_image image(320,200);
  image_drawer draw(image);

  renderBA81(320, 200, pixels, lines, &draw);

  image.save_image("output.bmp");
  usleep(100000);

}




