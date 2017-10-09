#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp.h"

int32_t get_padding(int32_t w){
    return (4 - 3*w % 4) % 4;
}

void read_row(src_t **data, int32_t i, int32_t width, FILE *f){
    for (int32_t j = 0; j < width; j++){
        fread(&data[i][j].b, 1, 1, f);
        fread(&data[i][j].g, 1, 1, f);
        fread(&data[i][j].r, 1, 1, f);
    }
    
    int32_t extrabyte = get_padding(width);
    int32_t tmp;
    
    fread(&tmp, 1, extrabyte, f);
}

src_t** src_alloc(int32_t height, int32_t width){

    src_t **data = (src_t**)malloc(sizeof(src_t *)*height);

    for (int32_t i = 0; i < height; i++)
        data[i] = (src_t *)malloc(sizeof(src_t)*width);

    return data;
}

BMPHeader* header_load(FILE *f){
    
    BMPHeader *res = (BMPHeader*)malloc(sizeof(BMPHeader));
    
    fread(&(res->bfType), 2, 1, f);
    fread(&(res->bfSize), 4, 1, f);
    fread(&(res->bfReserved1), 2, 1, f);
    fread(&(res->bfReserved2), 2, 1, f);
    fread(&(res->bfOffBits), 4, 1, f);
    
    fread(&(res->biSize), 4, 1, f);
    fread(&(res->biWidth), 4, 1, f);
    fread(&(res->biHeight), 4, 1, f);
    fread(&(res->biPlanes), 2, 1, f);
    fread(&(res->biBitCount), 2, 1, f);
    fread(&(res->biCompression), 4, 1, f);
    fread(&(res->biSizeImage), 4, 1, f);
    fread(&(res->biXPelsPerMeter), 4, 1, f);
    fread(&(res->biYPelsPerMeter), 4, 1, f);
    fread(&(res->biClrUsed), 4, 1, f);
    fread(&(res->biClrImportant), 4, 1, f);
    
    return res;
}

image_t* image_alloc(BMPHeader *header, src_t **data){
    image_t* res = (image_t *)malloc(sizeof(image_t));
    
    res->data = data;
    res->header = header;
    
    return res;
}

BMPHeader* header_copy(BMPHeader *header){
	BMPHeader *newheader = (BMPHeader*)malloc(sizeof(BMPHeader));
	newheader->bfType = header->bfType;
	newheader->bfSize = header->bfSize;
	newheader->bfReserved1 = header->bfReserved1;
	newheader->bfReserved2 = header->bfReserved2;
	newheader->bfOffBits = header->bfOffBits;
	
	newheader->biSize = header->biSize;
	newheader->biWidth = header->biWidth;
	newheader->biHeight = header->biHeight;
	newheader->biPlanes = header->biPlanes;
	newheader->biBitCount = header->biBitCount;
	newheader->biCompression = header->biCompression;
	newheader->biSizeImage = header->biSizeImage;
	newheader->biXPelsPerMeter = header->biXPelsPerMeter;
	newheader->biYPelsPerMeter = header->biYPelsPerMeter;
	newheader->biClrUsed = header->biClrUsed;
	newheader->biClrImportant = header->biClrImportant;
	return newheader;
}

image_t* bmp_load(char *input){
    int32_t isneg = 0;
    FILE *f = fopen(input, "rb");
    
    if (f == NULL){
        printf("No such file\n");
        exit(1);
    }
    
    BMPHeader* header = header_load(f);
    
    if (header->biHeight < 0){
        isneg = 1;
        header->biHeight *= -1;
    }
    
    src_t **data = src_alloc(header->biHeight, header->biWidth);
    fseek(f, header->bfOffBits, SEEK_SET);
    
    if (!isneg){
        
        for (int32_t i = 0; i < header->biHeight; i++)
            read_row(data, i, header->biWidth, f);
        
    } else {
        
        for (int32_t i = header->biHeight - 1; i >= 0; i--)
            read_row(data, i, header->biWidth, f);
        
    }
    
    image_t *loaded_image = image_alloc(header, data);
    
    fclose(f);
    return loaded_image;
}

crd_t* get_args(int32_t argc, char *argv[]){
    if (argc != 7){
        printf("Wrong number of arguments!\n");
        exit(1);
    }
    int32_t x0 = atoi(argv[3]);
    int32_t y0 = atoi(argv[4]);
    int32_t x1 = atoi(argv[5]);
    int32_t y1 = atoi(argv[6]);
    crd_t *res = (crd_t*)malloc(sizeof(crd_t));
    res->x0 = x0;
    res->y0 = y0;
    res->x1 = x1;
    res->y1 = y1;
    return res;
}

void checkcrd(image_t *image, crd_t *crd){
    int32_t h = image->header->biHeight;
    int32_t w = image->header->biWidth;
    if (!(crd->x0 >= 0 && crd->x0 < w && crd->x1 >= 0 && crd->x1 < w)){
        printf("Wrong x0, x1 coordinates!\n");
        exit(1);
    }
    if (!(crd->y0 >= 0 && crd->y0 < h && crd->y1 >= 0 && crd->y1 < h)){
        printf("Wrong y0, y1 coordinates!\n");
        exit(1);
    }
    if (!(crd->x0 <= crd->x1 && crd->y0 <= crd->y1)){
        printf("Wrong position of points\n");
        exit(1);
    }

    crd->y0 = h - crd->y0 - 1;
    crd->y1 = h - crd->y1 - 1;
}

void freeimage(image_t *image){

    for (int32_t i = 0; i < image->header->biHeight; i++)
        free(image->data[i]);

    free(image->data);
    free(image->header);
    free(image);
}

image_t* crop(crd_t *crd, image_t *image){
    int32_t new_height = crd->y0 - crd->y1 + 1;
    int32_t new_width = crd->x1 - crd->x0 + 1;
    src_t **new_data = src_alloc(new_height, new_width);

    for (int32_t i = crd->y1; i <= crd->y0; i++){
        for (int32_t j = crd->x0; j <= crd->x1; j++){
            new_data[i - crd->y1][j - crd->x0] = image->data[i][j];
        }
    }
    
	BMPHeader *newheader = header_copy(image->header);
    image_t *croped_image = image_alloc(newheader, new_data);
    croped_image->header->biHeight = new_height;
    croped_image->header->biWidth = new_width;

    return croped_image;
}

image_t* rotate(image_t *image){
    int32_t h = (image->header)->biWidth;
    int32_t w = (image->header)->biHeight;
    
    src_t **out_data = src_alloc(h, w);
    
    for (int32_t i = 0; i < h; i++)
        for (int32_t j = 0; j < w; j++)
            out_data[i][j] = image->data[j][h - 1 - i];

	BMPHeader *newheader = header_copy(image->header);
    image_t *rotated_image = image_alloc(newheader, out_data);

    rotated_image->header->biHeight = h;
    rotated_image->header->biWidth = w;
    rotated_image->header->bfSize = headerSize + h*(3*w + get_padding(w));
	rotated_image->header->biSizeImage = h*(3*w + get_padding(w));
    return rotated_image;
}

void save_bmp(char *output, image_t *image){
    int32_t h = image->header->biHeight;
    int32_t w = image->header->biWidth;
    FILE *f = fopen(output, "wb");

    fwrite(&(image->header->bfType), 2, 1, f);
    fwrite(&(image->header->bfSize), 4, 1, f);
    fwrite(&(image->header->bfReserved1), 2, 1, f);
    fwrite(&(image->header->bfReserved2), 2, 1, f);
    fwrite(&(image->header->bfOffBits), 4, 1, f);

    fwrite(&(image->header->biSize), 4, 1, f);
    fwrite(&(image->header->biWidth), 4, 1, f);
    fwrite(&(image->header->biHeight), 4, 1, f);
    fwrite(&(image->header->biPlanes), 2, 1, f);
    fwrite(&(image->header->biBitCount), 2, 1, f);
    fwrite(&(image->header->biCompression), 4, 1, f);
    fwrite(&(image->header->biSizeImage), 4, 1, f);
    fwrite(&(image->header->biXPelsPerMeter), 4, 1, f);
    fwrite(&(image->header->biYPelsPerMeter), 4, 1, f);
    fwrite(&(image->header->biClrUsed), 4, 1, f);
    fwrite(&(image->header->biClrImportant), 4, 1, f);
    
    for (int32_t i = 0; i < h; i++){
        for (int32_t j = 0; j < w; j++){
            fwrite(&(image->data[i][j].b), 1, 1, f);
            fwrite(&(image->data[i][j].g), 1, 1, f);
            fwrite(&(image->data[i][j].r), 1, 1, f);
        }
        int32_t extrabyte = get_padding(w);
        int32_t tmp = 0;

        fwrite(&tmp, 1, extrabyte, f);
    }

    fclose(f);
}
