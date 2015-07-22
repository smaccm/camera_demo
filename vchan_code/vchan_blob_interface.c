/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include "includes/vmm_manager.h"
#include "includes/vchan_copy.h"
#include "includes/libvchan.h"

#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

static libvchan_t *con;

int send_blob(int l, int r, int t, int b) {
	size_t sz;
    float angles[2];

	printf("vchan: packet start\n");
	sz = libvchan_data_ready(con);
	printf("vchan: send packet\n");

    int xmid = ((r + l) / 2) - 160;
    int ymid = ((b + t) / 2) - 100;

    ymid = -1.0*ymid; //sign change

    angles[0] = ((float) xmid)*(0.00410666);
    angles[1] = ((float) ymid)*(0.00410666);

    printf("x angle: %f rads\n", angles[0]);
    printf("y angle: %f rads\n", angles[1]);
    printf("\n");

    while(libvchan_buffer_space(con) < 2*sizeof(float));
    sz = libvchan_send(con, angles, 2*sizeof(float));
    if(sz < 2*sizeof(float)) {
    	printf("--BAD PACKET -- SEND\n");
    	return -1;
    }

	printf("vchan: waiting for ack..\n");

	libvchan_wait(con);
    return 0;
}

int vchan_init(void){

	printf("vchan: Creating connection in image\n");
	con = libvchan_client_init(50, 25);
	assert(con != NULL);
	printf("vchan: Connection Established!\n");

	return 0;
}

void vchan_close(void){
    libvchan_close(con);
}

