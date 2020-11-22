#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

typedef struct {
   uint8_t  idlength;
   uint8_t  colourmaptype;
   uint8_t  datatypecode;
   uint16_t colourmaporigin;
   uint16_t colourmaplength;
   uint8_t  colourmapdepth;
   uint16_t x_origin;
   uint16_t y_origin;
   uint16_t width;
   uint16_t height;
   uint8_t  bitsperpixel;
   uint8_t  imagedescriptor;
} TGAHeader;

typedef struct 
{
    uint8_t B;
    uint8_t G;
    uint8_t R;
}RGBcolor;

typedef struct
{
    int rows;
    int cols;

    int size;

    RGBcolor* data;

} Matrix;

