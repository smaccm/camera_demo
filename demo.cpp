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

#define BLOCK_BUFFER_SIZE    25

// Pixy Block buffer // 
struct Block blocks[BLOCK_BUFFER_SIZE];
void handle_SIGINT(int unused)
{
  // On CTRL+C - abort! //

  printf("\nBye!\n");
  exit(0);
}
//defined in pixy.cpp
extern SmaccmInterpreter interpreter;
int main(int argc, char * argv[])
{
  int      index;
  int      blocks_copied;
  int      pixy_init_status;

 // if(argc != 2){
 //   std::cerr << "Usage: demo <port>" << std::endl;
 //     return 1;
 // }

  // Catch CTRL+C (SIGINT) signals //
  signal(SIGINT, handle_SIGINT);

  printf("Hello Pixy:\n libpixyusb Version: %s\n", __LIBPIXY_VERSION__);

  int32_t response;
  uint32_t fourcc;
  int8_t renderflags;
  uint16_t width, height, sentWidth, sentHeight;
  uint32_t numPixels;
  

  //wait for client connection
  interpreter.connect();
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
  
  //this is magic. I have no idea why I need to do this but I do
  printf("running prog\n");
  pixy_command("runprog", 0x01, 8, END, &response, END);
  printf("sent running prog\n");
  pixy_command("cam_setAWB", 0x01, 1, 0, &response, 0);
  //pixy_command("cam_setMode",0x01, 1, END, &response, END);
  //usleep(1000000);
  
  int t = 0;
  for(;;){

    usleep(100000000);
  }
     
}  
   
   
   
   
   
   
   
   
