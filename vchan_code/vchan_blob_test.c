#include "vchan_blob_interface.h"
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void handle_SIGINT(int unused)
{
  // On CTRL+C - abort! //

  printf("\nBye!\n");
  vchan_close();
  exit(0);
}

int main(int argc, char ** argv){
  int i;

  // Catch CTRL+C (SIGINT) signals //
  signal(SIGINT, handle_SIGINT);

  vchan_init();
  for(i = 0 ;; i++){
    send_blob(i+1,i+2,i+3,i+4);
    sleep(1);
  }
}
