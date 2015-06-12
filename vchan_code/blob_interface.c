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
	vchan_packet_t pak;
	int x, i;
	char fnack;
        static int numSends = 0;
        static int calledOnce = 0;

        if(calledOnce)
          return 0;
        calledOnce = 1;
	printf("vchan: packet start\n");

	/* Check that buffer data is correct */
	sz = libvchan_data_ready(con);
	if(sz != 0) {
		printf("error: incorrect start packet buffer size (data ready) %d\n", sz);
		return -1;
	}

	sz = libvchan_buffer_space(con);
	if(sz != FILE_DATAPORT_MAX_SIZE) {
		printf("error: incorrect start packet buffer size (bspace) %d\n", sz);
		return -1;
	}

	/* Start */

    int num_packets = 1;
	sz = libvchan_send(con, &num_packets, sizeof(int));
	if(sz < sizeof(int)) {
		printf("--BAD PACKET NUM -- SEND\n");
		return -1;
	}

	printf("vchan: send packet\n");
        pak.pnum = numSends++;
        pak.datah[0] = l;
        pak.datah[1] = r;
        pak.datah[2] = t;
        pak.datah[3] = b;
        pak.guard = TEST_VCHAN_PAK_GUARD;

        while(libvchan_buffer_space(con) < sizeof(pak));
        sz = libvchan_send(con, &pak, sizeof(pak));
        if(sz < sizeof(pak)) {
        	printf("--BAD PACKET -- SEND\n");
        	return -1;
        }
        return 0;
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
//	printf("vchan: waiting for ack..\n");
//
//	libvchan_wait(con);
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
