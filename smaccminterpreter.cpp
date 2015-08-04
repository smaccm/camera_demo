#include "smaccminterpreter.hpp"

#define PACKET_SIZE 65000

SmaccmInterpreter::SmaccmInterpreter() : 
  m_blobs() {
}

int SmaccmInterpreter::connect(){
  connect(SERVICE_PORT);
}

int SmaccmInterpreter::connect(int port){
  if ((recvfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("cannot create socket\n");
    exit(0);
  }
  
  // initiate vchan
  vchan_init();
  
  // bind the socket to any valid IP address and a specific port
  memset((char *)&myaddr, 0, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(port);
  
  if (bind(recvfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
    perror("bind failed");
    return 0;
  }
  
  printf("waiting for client connection...\n");
  char val;
  socklen_t addrlen = sizeof(remaddr);
  int recvlen = recvfrom(recvfd, &val, 1, 0, (struct sockaddr *)&remaddr, &addrlen);
  printf("Client connected at address %s, port %d\n", inet_ntoa(remaddr.sin_addr), ntohs(remaddr.sin_port));
  fNewFrame = 1;
  remove("attack.rgb");
  boost::thread frameSenderThread(boost::bind(&SmaccmInterpreter::sendFrame, this));
}

void SmaccmInterpreter::compressFrame(){
  const int JPEG_QUALITY = 75;
  const int COLOR_COMPONENTS = 3;
  long unsigned int jpegSize = 0;
  unsigned char* compressedImage = 0;

  tjhandle jpegCompressor = tjInitCompress();
  tjCompress2(jpegCompressor, processedPixels, WIDTH, 0, HEIGHT, TJPF_RGB,
            &compressedImage, &jpegSize, TJSAMP_444, JPEG_QUALITY,
            TJFLAG_FASTDCT);
  tjDestroy(jpegCompressor);
  
  memcpy(compressedPixels, compressedImage, jpegSize);
  compressedLength = jpegSize;
  tjFree(compressedImage);
}

void SmaccmInterpreter::sendFrame() {
  boost::system::error_code ignored_error;
  int fFrameSent = 0;
  for(;;) {
    if(fNewFrame) {
      imageMutex.lock();
      renderCMV1(0, cmodelsLen, cmodels, width, height, frame_len, pFrame);
      corruptFrame();
      compressFrame();
      
      if (!sendto(recvfd, compressedPixels, compressedLength, 0,
		  (struct sockaddr *)&remaddr, sizeof(struct sockaddr_in))) {
	perror("sendto");
      }
      
      fNewFrame = 0;
      fFrameSent = 1;
      imageMutex.unlock();
    }
    usleep(10000);
  }
}

void SmaccmInterpreter::corruptFrame(){
  // Corrupt the stream if under attack
  static char corrupted[WIDTH*HEIGHT] = {0};
  static int pixelsToCorrupt = 0;
  static int attackFrame = 0;
  static int numAttackFrames;
  static uint8_t *vaddr = NULL;

  if (vaddr == NULL) {
    FILE *fp = fopen("attack.rgb", "rb");
    if (fp != NULL) {
      struct stat statbuf;
      fstate(fp, &statbuf);
      vaddr = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
      numAttackFrames = statbuf.st_size / (3 * WIDTH * HEIGHT);
    }
  }

  if (vaddr != NULL) {
    int frameOffset = 3 * WIDTH * HEIGHT * attackFrame;
    attackFrame = (attackFrame + 1) % numAttackFrames;

    if (pixelsToCorrupt < 10000) {
      pixelsToCorrupt += 200;
    }

    for (int i = 0; i < pixelsToCorrupt; i++) {
      corrupted[rand() % (WIDTH * HEIGHT)] = 1;
    }

    for (int i = 0; i < WIDTH*HEIGHT; i++) {
      if (corrupted[i]) {
        memcpy(processedPixels, vaddr + frameOffset + 3*i, 3);
      }
    }
  }
}

void SmaccmInterpreter::interpolateBayer(unsigned int width, unsigned int x, unsigned int y, unsigned char *pixel, unsigned int &r, unsigned int &g, unsigned int &b)
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

int SmaccmInterpreter::renderBA81(uint16_t width, uint16_t height, uint8_t *frame, uint8_t * lines, uint32_t numBlobs, BlobA * blobs)
{
  uint16_t x, y;
  uint8_t *line;
  uint32_t r, g, b;

  // don't render top and bottom rows, and left and rightmost columns because of color
  // interpolation

  // skip first line
  frame += width;
  for (y=1; y<height-1; y++)
  {
    line = (uint8_t *)(lines + (y-1)*width*3);
    frame++;
    for (x=1; x<width-1; x++, frame++)
    {
      interpolateBayer(width, x, y, frame, r, g, b);
      *line++ = (uint8_t)r;
      *line++ = (uint8_t)g;
      *line++ = (uint8_t)b;
    }
    frame++;
  }

  int blobIndex;
  int ll, rr, tt, bb;
  int largestPerim = 0;
  for(blobIndex = 0; blobIndex < numBlobs; blobIndex++){
    int l, r, t, b;
    int perim;
    l = blobs[blobIndex].m_left*2;
    r = blobs[blobIndex].m_right*2;
    t = blobs[blobIndex].m_top*2;
    b = blobs[blobIndex].m_bottom*2;
    
    assert(l <= 320);
    assert(r <= 320);
    assert(t <= 200);
    assert(b <= 200);
    assert(l <= r);
    assert(t <= b);

    // Identify largest bounding box
    perim = r - l + b - t;
    if(largestPerim < perim){
      largestPerim = perim;
      ll = l;
      rr = r;
      tt = t;
      bb = b;
    }

    // Draw bounding box
    int i;
    for(i = l*3; i <= 3*r; i = i + 3){
      lines[(width*3*t) + i] = 0;  
      lines[(width*3*t) + i+1] = 255;  
      lines[(width*3*t) + i+2] = 0;  
      lines[(width*3*b) + i] = 0;  
      lines[(width*3*b) + i+1] = 255;  
      lines[(width*3*b) + i+2] = 0;  
    }
    
    for(i = t*width*3; i <= b*width*3; i = i + width*3){
      lines[i + l*3] = 0;  
      lines[i + l*3+1] = 255;  
      lines[i + l*3+2] = 0;  
      lines[i + r*3] = 0;  
      lines[i + r*3+1] = 255;  
      lines[i + r*3+2] = 0;  
    }
  }

  if(largestPerim != 0){
    send_blob(ll, rr, tt, bb);
  }
  return 0;
}

int SmaccmInterpreter::renderCMV1(uint8_t renderFlags, uint32_t cmodelsLen, float *cmodels, uint16_t width, uint16_t height, uint32_t frameLen, uint8_t *frame) 
{
    int i;
    uint32_t numBlobs, numCCBlobs;
    BlobA *blobs;
    BlobB *ccBlobs;
    uint32_t numQvals;
    uint32_t *qVals;

    if (cmodelsLen>=sizeof(ColorModel)*NUM_MODELS/sizeof(float)) // create lookup table
    {   
        m_blobs.m_blobs->m_clut->clear();
        for (i=0; i<NUM_MODELS; i++, cmodels+=sizeof(ColorModel)/sizeof(float))
            m_blobs.m_blobs->m_clut->add((ColorModel *)cmodels, i+1);
    }   

    m_blobs.process(Frame8(frame, width, height), &numBlobs, &blobs, &numCCBlobs, &ccBlobs, &numQvals, &qVals);

    renderBA81(width, height, frame, processedPixels, numBlobs, blobs);

    return 0;
}


void SmaccmInterpreter::interpret_data(void * chirp_data[])
{
  uint8_t  chirp_message;
  uint32_t chirp_type;
  static int t = 0;

  if (chirp_data[0]) {
    chirp_message = Chirp::getType(chirp_data[0]);
    switch(chirp_message) {
      case CRP_TYPE_HINT:
        chirp_type = * static_cast<uint32_t *>(chirp_data[0]);
        switch(chirp_type) {
          case FOURCC('B', 'A', '8', '1'):
            break;
          case FOURCC('C', 'C', 'Q', '1'):
            printf("got CCQ1\n");
            break;
          case FOURCC('C', 'C', 'B', '1'):
            printf("got CCB1\n");
             break;
          case FOURCC('C', 'C', 'B', '2'):
            printf("got CCB2\n");
             break;
          case FOURCC('C', 'M', 'V', '1'):
            
            if(imageMutex.try_lock()){
              cmodelsLen = *(uint32_t *)chirp_data[2];
              cmodels = (float *)chirp_data[3];
              width = *(uint16_t *)chirp_data[4];
              height = *(uint16_t *)chirp_data[5];
              frame_len = *(uint32_t *)chirp_data[6];
              pFrame = (uint8_t *)chirp_data[7];
	      assert(width == WIDTH);
	      assert(height == HEIGHT);
	      assert(frame_len = width*height);
	      
              fNewFrame = 1;
              imageMutex.unlock();
            }else{
	      //printf("didn't get lock\n");
            }
            break;
          default:
            printf("libpixy: Chirp hint [%u] not recognized.\n", chirp_type);
            break;
        }

        break;

      case CRP_HSTRING:
        printf("I see something!\n");
        break;
      
      default:
        fprintf(stderr, "libpixy: Unknown message received from Pixy: [%u]\n", chirp_message);
	break;
    }
  } 
}
