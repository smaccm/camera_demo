
#ifndef _BLOB_INTERFACE_H
#define _BLOB_INTERFACE_H 

extern "C" {

int vchan_init(void);
void vchan_close(void);
int send_blob(int l, int r, int t, int b);
};
#endif

