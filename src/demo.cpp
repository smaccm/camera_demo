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
#include "pixy.h"
#include "smaccminterpreter.hpp"
#include "bbox.h"

#define BLOCK_BUFFER_SIZE    25

// Pixy Block buffer // 
struct Block blocks[BLOCK_BUFFER_SIZE];

//defined in pixy.cpp
extern SmaccmInterpreter interpreter;

int main(int argc, char * argv[])
{
  int index;
  int blocks_copied;
  int pixy_init_status;

  printf("Hello Pixy:\n libpixyusb Version: %s\n", __LIBPIXY_VERSION__);

  int32_t response;
  uint32_t fourcc;
  int8_t renderflags;
  uint16_t width, height, sentWidth, sentHeight;
  uint32_t numPixels;

  char const *ip = "255.255.255.255";
  int port = 4000;
  if(argc >= 2) {
    ip = argv[1];
  }
  if(argc >= 3) {
    port = atoi(argv[2]);
  }
  
  interpreter.connect(ip, port);

  // Connect to Pixy
  pixy_init_status = pixy_init();

  // Was there an error initializing pixy?
  if(!pixy_init_status == 0)
  {
    // Error initializing Pixy
    printf("pixy_init(): ");
    pixy_error(pixy_init_status);
    return pixy_init_status;
  }
  
  printf("Resetting camera\n");
  pixy_command("stop", END, &response, END);

  printf("Starting camera tracking\n");
  while (1) {
    sleep(1);
    pixy_command("runprogArg", UINT8(8), UINT32(1), END, &response, END);
  }
}  
