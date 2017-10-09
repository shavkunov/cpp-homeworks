#ifndef _BMP_H_
#define _BMP_H_

#include <stdint.h>

#define headerSize 54

struct crd_s{
    
    int32_t x0;
    int32_t y0;
    int32_t x1;
    int32_t y1;
    
};

struct src_s{
    
    int32_t b;
    int32_t g;
    int32_t r;
    
};

struct BMPHeader{
    
    int16_t bfType;
    int32_t bfSize;
    int16_t bfReserved1;
    int16_t bfReserved2;
    int32_t bfOffBits;
    
    int32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    int16_t biPlanes;
    int16_t biBitCount;
    int32_t biCompression;
    int32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    int32_t biClrUsed;
    int32_t biClrImportant;
    
};

typedef struct crd_s crd_t;
typedef struct src_s src_t;
typedef struct BMPHeader BMPHeader;

struct image_s{
    
    BMPHeader *header;
    src_t **data;
    
};

typedef struct image_s image_t;

image_t* bmp_load(char *input);
image_t* crop(crd_t *crd, image_t *image);
image_t* rotate(image_t *image);
void save_bmp(char *output, image_t *image);
void freeimage(image_t *image);
void checkcrd(image_t *in, crd_t *crd);
crd_t* get_args(int32_t argc, char *argv[]);

#endif
