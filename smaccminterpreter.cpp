#include "smaccminterpreter.hpp"

SmaccmInterpreter::SmaccmInterpreter(){
}

int SmaccmInterpreter::init(){
  PixyInterpreter::init();

 // acceptor = new tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), atoi(argv[1])));
  //socket(io_service);
  //acceptor.accept(socket);

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


int SmaccmInterpreter::renderBA81(uint16_t width, uint16_t height, uint8_t *frame, uint32_t * lines, bitmap_image &image)
{
    uint16_t x, y;
    uint32_t *line;
    uint32_t r, g, b;

    image_drawer drawer(image);
    // skip first line
    frame += width;

    // don't render top and bottom rows, and left and rightmost columns because of color
    // interpolation
    //QImage img(width-2, height-2, QImage::Format_RGB32);

    for (y=1; y<height-1; y++)
    {
        line = (unsigned int *)(lines + (y-1)*width);
        frame++;
        for (x=1; x<width-1; x++, frame++)
        {
            interpolateBayer(width, x, y, frame, r, g, b);
            *line++ = (0x40<<24) | (r<<16) | (g<<8) | (b<<0);
            drawer.pen_color((char)r,(char)g,(char)b);            
            drawer.plot_pixel(x,y);
        }
        frame++;
    }

    return 0;
}

