#include "bbox.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>

#define BBOX_PADDR 0xC0000000

void write_bbox(uint16_t left, uint16_t right, uint16_t bottom, uint16_t top) {
  static void *map_base = 0;
  if (map_base == 0) {
    int map_fd = open("/dev/mem", O_RDWR | O_SYNC);
    assert(map_fd != -1);

    map_base = mmap(0, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, map_fd, BBOX_PADDR);
    assert(map_base != (void *) -1);
  }

  volatile uint16_t *q = (uint16_t *) map_base;
  *q++ = left;
  *q++ = right;
  *q++ = bottom;
  *q++ = top;
}
