#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>


uint8_t pow(uint8_t base, uint8_t exp){
    uint8_t result = 1;
    for(size_t i = 0; i < exp; i++){
        result *= base;
    }
    return result;
}

uint8_t convertBinToDec(uint8_t* bits){
    uint8_t number = 0;
    for(size_t i = 0; i < 8; i++){
        number += bits[i] * pow(2, 7-i);
        bits[i] = 0;
    }
    return number;
}

uint64_t shareByteMethod(uint8_t *bufferIn, uint8_t *bufferOut, uint64_t size){
    uint8_t bits[8];
    uint8_t index = 0;
    uint64_t outPos = 0;
    uint8_t bit = 0;

    for(uint64_t i = 0; i < size; i++){
        for(uint8_t j = 0; j < (bufferIn[i] >> 4); j++){
            bits[index++] = 0;
            if(index >= 8){
                bufferOut[outPos++] = convertBinToDec(bits);
                index = 0;
            }
        }
        for(uint8_t j = 0; j < (bufferIn[i] & 0x0F); j++){
            bits[index++] = 1;
            if(index >= 8){
                bufferOut[outPos++] = convertBinToDec(bits);
                index = 0;
            }
        }
    }

    return outPos;
}

void singleByteMethod(){

}

int main(int argc, char** argv){
    if(argc != 3){
        printf("\nSyntax: %s <.pat file> <output file>", argv[0]);
        return 1;
    }

    FILE* patFile = fopen(argv[1], "rb");
    if(!patFile){
        printf("\nFile %s does not exist!", argv[1]);
    }

    uint64_t size = 0;
    while(fgetc(patFile) != EOF){
        size++;
    }
    rewind(patFile);

    printf("\nInput filesize: %lld Byte\n", size);

    uint8_t *bufferIn = (uint8_t*) malloc(size);
    fread(bufferIn, 1, size, patFile);

    uint8_t *bufferOut = malloc(size);

    uint64_t decompressedSize = shareByteMethod(bufferIn, bufferOut, size);

    
    printf("Output filesize: %lld Byte\n", decompressedSize);
    FILE* output = fopen(argv[2], "wb");
    fwrite(bufferOut, 1, decompressedSize, output);

    free(bufferIn);
    free(bufferOut);
    fclose(patFile);
    fclose(output);
    
    return 0;
}