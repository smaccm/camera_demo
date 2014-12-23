#include "smaccminterpreter.hpp"

SmaccmInterpreter::SmaccmInterpreter() : 
  acceptor(io_service, tcp::endpoint(tcp::v4(), 1337)),
  socket(io_service) {

}

int SmaccmInterpreter::connect(){
  pImage = new bitmap_image(sentWidth, sentHeight);
  //set the flag saying we have not captured an image yet
  fNewImage = 0;
  printf("waiting for client connection...\n");
  acceptor.accept(socket);
  printf("Client connected!\n");
  socket.set_option(tcp::no_delay(false));
  //start the thread that will be used to send frames
  boost::thread frameSenderThread(boost::bind(&SmaccmInterpreter::sendFrame, this));
}

void SmaccmInterpreter::sendFrame(){
  boost::system::error_code ignored_error;
  static int i = 0;
  for(;;){
    usleep(30000);
    imageMutex.lock();
    if(fNewImage){
      //pImage->save_image("output" + boost::to_string(i++) + ".bmp");
      boost::asio::write(socket, boost::asio::buffer(processedPixels, sentWidth*sentHeight*sizeof(uint8_t)*3),
        boost::asio::transfer_all(), ignored_error);
      fNewImage = 0;
    }
    imageMutex.unlock();
    waitForResponse();
  }
}

void SmaccmInterpreter::waitForResponse(){
  boost::array<char, 1> buf;
  boost::system::error_code error;
  size_t len = 0;

  len = socket.read_some(boost::asio::buffer(buf), error);
  if (error == boost::asio::error::eof){
    printf("Client disconnected. Waiting for reconnect...\n");
    socket.close();
    acceptor.accept(socket);
    printf("Client reconnected!\n");
    socket.set_option(tcp::no_delay(false));  
  }else if (error){
    printf("An error occurred. Waiting for client to reconnect...\n");
    acceptor.accept(socket);
    printf("Client reconnected!\n");
    socket.set_option(tcp::no_delay(false));
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


//with is in pixels not bytes!
int SmaccmInterpreter::renderBA81(uint16_t width, uint16_t height, uint8_t *frame, uint8_t * lines, bitmap_image *pImage)
{
    uint16_t x, y;
    uint8_t *line;
    uint32_t r, g, b;
    
    //if(imageMutex.try_lock()){
      imageMutex.lock();
      image_drawer drawer(*pImage);
      // skip first line
      frame += width;

      // don't render top and bottom rows, and left and rightmost columns because of color
      // interpolation

      for (y=1; y<height-1; y++)
      {
          line = (uint8_t *)(lines + (y-1)*width*3);
          frame++;
          for (x=1; x<width-1; x++, frame++)
          {
              interpolateBayer(width, x, y, frame, r, g, b);
              //*line++ = (0x40<<24) | (r<<16) | (g<<8) | (b<<0);
              *line++ = (uint8_t)r;
              *line++ = (uint8_t)g;
              *line++ = (uint8_t)b;
              drawer.pen_color((char)r,(char)g,(char)b);            
              drawer.plot_pixel(x,y);
          }
          frame++;
      }
      fNewImage = 1;
      imageMutex.unlock();
    //}
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
            uint16_t width, height;
            uint32_t frame_len;
            uint8_t * pFrame;
			width = *(uint16_t *)chirp_data[2];
			height = *(uint16_t *)chirp_data[3];
			frame_len = *(uint32_t *)chirp_data[4];
			pFrame = (uint8_t *)chirp_data[5];
			assert(width == sentWidth);
			assert(height == sentHeight);
			assert(frame_len = width*height);

            //printf("rendering: %d\n", t++);
			
            renderBA81(width, height, pFrame, processedPixels, pImage);
            break;
          case FOURCC('C', 'C', 'Q', '1'):
            break;
          case FOURCC('C', 'C', 'B', '1'):
            //interpret_CCB1(chirp_data + 1);
            break;
          case FOURCC('C', 'C', 'B', '2'):
            //interpret_CCB2(chirp_data + 1);
            break;
          case FOURCC('C', 'M', 'V', '1'):
            break;
          default:
            printf("libpixy: Chirp hint [%u] not recognized.\n", chirp_type);
            break;
        }

        break;

      case CRP_HSTRING:

        break;
      
      default:
       
       fprintf(stderr, "libpixy: Unknown message received from Pixy: [%u]\n", chirp_message);
       break;
    }
  } 
}
