

#include "../include/bmp.h"

using namespace std;

/**
 * Extraído de: https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file
 * */
Image readBMP(const char* filename){
    
    
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
    
    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];
    Image im;
    
    im.width = width;
    im.height = height;

    unsigned short int  bits = *(short *) &info[28];
    if(bits != 8){
        printf("Error, The image have to be of 8 Bits");
        exit (EXIT_FAILURE);
    }
    
    int row_padded = (width + 3) & (~3);
    int size = width * height;
    unsigned char* data = new unsigned char[size];
    unsigned int matrix_addr = *(int *) &info[10];
    fseek(f, matrix_addr-54, SEEK_CUR);
    fread(data, sizeof(unsigned char), size, f); 
    
    fclose(f);
    
    im.arrayOfPixels = data;
    im.height = height;
    im.width = width;
    
    return im;
}



void algorithm(Image im1, Image im2){
    
    int indexResults = 0;
    ValueResult* result[(im1.height/16) * (im1.width/16)];
    
    for(int i = 0; i < im1.height;i+=16){
        for(int j = 0; j < im1.width; j+=16){

            ValueResult* dataFrame = new ValueResult();
            dataFrame->minimum = 2147483647; // Maximum value for a variable of type int.
            
            for(int u = 0; u < im2.height-16; u++){
                for(int l = 0; l < im2.width-16 ; l++){

                    int summation = 0;
                    for(int k = 0; k < 16; k++){
                        for(int y = 0; y < 16; y++){
                            summation += abs(im1.arrayOfPixels[getIndex(i+k,j+y,im1.width)] - im2.arrayOfPixels[getIndex(u+k,l+y,im2.width)]);
                        }
                    }
                    
                    if(summation < dataFrame->minimum){
                        dataFrame->minimum = summation;
                        dataFrame->iFrame1 = i;
                        dataFrame->jFrame1 = j;
                        dataFrame->iFrame2 = u;
                        dataFrame->jFrame2 = l;           

                        if(summation == 0) goto endFrame2;
                    }
                }
            }
            endFrame2:
            result[indexResults] = dataFrame;
            indexResults += 1;
        }
    }
}


int main(){
    const char *f1 = "../imagenes/frame1.bmp";
    const char *f2 = "../imagenes/frame2.bmp";

    Image im1 = readBMP(f1);    
    Image im2 = readBMP(f2);
    
    clock_t begin = clock();
    algorithm(im1,im2);
    clock_t end = clock();

    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    
    printf("The time in serial is %.6f minutes", elapsed_secs/60);

    return 0;
}