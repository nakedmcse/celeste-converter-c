#include <stdio.h>
#include <string.h>
#include "convert.h"

int main(int argc, char **argv) {
    if(argc < 4) {
        printf("Usage: celeste-converter [command] [input] [output]\n");
        printf("Commands:\n");
        printf("\tdata2png\tConvert from Celeste DATA format to PNG\n");
        printf("\tpng2data\tConvert from PNG to Celeste DATA format\n");
        return 0;
    }

    if(strncmp(argv[1], "data2png", 8) == 0 || strncmp(argv[1], "png2data", 8) == 0) convert(argv[2], argv[3], argv[1]);
    else printf("Unknown command %s\n", argv[1]);
    return 0;
}