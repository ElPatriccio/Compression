#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>

typedef enum {
    none,
    zeroes,
    ones,
} Mode;


void convertToBits(uint8_t number, int8_t* out){
    size_t i = 8;
    while(number){
        out[i-1] = number % 2;
        i--;
        number /= 2;
    }
    while(i){
        out[i-1] = 0;
        i--;
    }
    return;
}

uint64_t getFileSize(FILE* file){
    uint64_t size = 0;
    while(fgetc(file) != EOF){
        size++;
    }
    rewind(file);
    return size;
}

void checkCompressedBigger(uint64_t compressedSize, uint64_t uncompressedSize){
    if(compressedSize >= uncompressedSize){
        printf("[WARNING] Uncompressed File is smaller than compressed version, dont use this compression method! %lld Byte -> %lld Byte\n", uncompressedSize, compressedSize);
    }
}

uint64_t shareByteMethod(uint8_t *bufferIn, uint8_t *bufferOut, uint64_t size){
    int8_t bits[8];
    uint8_t prev1 = 0;
    uint8_t prev0 = 0;
    uint64_t posOut = 0;
    Mode mode = zeroes;

    for(uint64_t i = 0; i < size; i++){
        convertToBits(bufferIn[i], bits);
        for(size_t j = 0; j < 8; j++){
            if(bits[j] == 1){
                if(mode == zeroes){
                    bufferOut[posOut] = prev0 << 4;
                    prev0 = 0;
                }
                mode = ones;
                if(++prev1 >= 15){
                    bufferOut[posOut++] |= 15;
                    bufferOut[posOut] &= 0;
                    prev1 -= 15;  
                }
            }

            else if (bits[j] == 0){
                if(mode == ones){
                    bufferOut[posOut++] |= prev1;
                    prev1 = 0;
                }
                mode = zeroes;
                if(++prev0 >= 15){
                    bufferOut[posOut++] = 15 << 4;
                    prev0 -= 15;
                }
            }
        }
    }
    if(prev0){
        bufferOut[posOut++] = prev0 << 4;
    }
    if(prev1){
        bufferOut[posOut++] |= prev1;
    }
    return posOut;

}
void singleByteMethod(){

}

int main(int argc, char**argv){
    if(argc < 2 || argc > 3){
        printf("Syntax: %s <filename> [<filename output>]", argv[0]);
        return 1;
    }

    FILE* uncompressed;
    FILE* compressed;

    uncompressed = fopen(argv[1], "rb");
    if(!uncompressed){
        printf("%s is not a file!", argv[1]);
    }
    
    uint64_t size = getFileSize(uncompressed);

    printf("\nInput filesize: %lld Byte\n", size);
    uint8_t *bufferIn = (uint8_t*) malloc(size);
    fread(bufferIn, 1, size, uncompressed);

    char *filename = "compressed.pat";
    if(argc == 3){
        filename = argv[2];
    }
    compressed = fopen(filename, "w+b");

    uint8_t *bufferOut = (uint8_t*) malloc(size*5);

    uint64_t compressedSize = shareByteMethod(bufferIn, bufferOut, size);
    
    fwrite(bufferOut, sizeof(uint8_t), compressedSize, compressed);
    printf("Output filesize: %lld Byte\n\n", compressedSize);
    checkCompressedBigger(compressedSize, size);

    free(bufferIn);
    free(bufferOut);
    fclose(compressed);
    fclose(uncompressed);
    
    return 0;
}
