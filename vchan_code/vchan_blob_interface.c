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
	int x, i;
	char fnack;
    static int numSends = 0;
    static int calledOnce = 0;
    float angles[2];

	printf("vchan: packet start\n");

	/* Check that buffer data is correct */
	sz = libvchan_data_ready(con);
//	if(sz != 0) {
//		printf("error: incorrect start packet buffer size (data ready) %d\n", sz);
//		return -1;
//	}
//
//	sz = libvchan_buffer_space(con);
//	if(sz != FILE_DATAPORT_MAX_SIZE) {
//		printf("error: incorrect start packet buffer size (bspace) %d\n", sz);
//		return -1;
//	}

	/* Start */

    //int num_packets = 1;
	//sz = libvchan_send(con, &num_packets, sizeof(int));
	//if(sz < sizeof(int)) {
	//	printf("--BAD PACKET NUM -- SEND\n");
	//	return -1;
	//}

	printf("vchan: send packet\n");

        while(libvchan_buffer_space(con) < 2*sizeof(float));
        printf("float size:%d", 2*sizeof(float));
        sz = libvchan_send(con, angles, 2*sizeof(float));
        if(sz < 2*sizeof(float)) {
        	printf("--BAD PACKET -- SEND\n");
        	return -1;
        }
//	for(x = 0; x < num_packets; x++) {
//		pak.pnum = x;
//		for(i = 0; i < 4; i++) {
//			pak.datah[i] = i + x;
//		}
//		pak.guard = TEST_VCHAN_PAK_GUARD;
//
//		while(libvchan_buffer_space(con) < sizeof(pak));
//		sz = libvchan_send(con, &pak, sizeof(pak));
//		if(sz < sizeof(pak)) {
//			printf("--BAD PACKET -- SEND\n");
//			return -1;
//		}
//	}
//
	printf("vchan: waiting for ack..\n");

	libvchan_wait(con);
        return 0;
//	sz = libvchan_read(con, &fnack, sizeof(char));
//	if(sz < sizeof(char) || ! fnack) {
//		return -1;
//	}
//
//	printf("vchan: pack end\n");
//	return 0;
}

int vchan_init(void){
	//int ecount = 0;

	printf("vchan: Creating connection in image\n");
	con = libvchan_client_init(50, 25);
	assert(con != NULL);
	printf("vchan: Connection Established!\n");

	//ecount += send_packet(ctrl, 2000);
	//printf("vchan: %d errors\n", ecount);

	return 0;
}

void vchan_close(void){
    libvchan_close(con);
}

