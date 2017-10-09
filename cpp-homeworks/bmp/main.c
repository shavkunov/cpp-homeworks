#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp.h"

int main(int32_t argc, char *argv[]){
    crd_t* crd = get_args(argc, argv);
    char* input_file = argv[1];
    image_t* input_image = bmp_load(input_file);
    
    checkcrd(input_image, crd);
    
    image_t *croped_image = crop(crd, input_image);

    image_t *rotated_image = rotate(croped_image);

    char *output_file = argv[2];
    save_bmp(output_file, rotated_image);

    free(crd);
    freeimage(input_image);
    freeimage(croped_image);
    freeimage(rotated_image);
    return 0;
}
