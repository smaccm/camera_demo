#include "vchan_blob_interface.h"
#include <unistd.h>

int main(int argc, char ** argv){
  int i;
  vchan_init();
  for(i = 0 ;; i++){
    send_blob(i+1,i+2,i+3,i+4);
    sleep(1000);
  }
}
