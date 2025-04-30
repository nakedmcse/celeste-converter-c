#include <stdio.h>
#include <string.h>
#include "convert.h"

void showHelp() {
    printf("Usage: celeste-converter [options] [command] [input] [output]\n");
    printf("Options:\n");
    printf("\t-h\tShow this help\n");
    printf("\t-v\tVerbose\n");
    printf("Commands:\n");
    printf("\tdata2png\tConvert from Celeste DATA format to PNG\n");
    printf("\tpng2data\tConvert from PNG to Celeste DATA format\n");
}

int main(int argc, char **argv) {
    if(argc < 4) {
        showHelp();
        return 0;
    }

    bool verbose = false;

    for(int i = 1; i <= argc-4; i++) {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            showHelp();
            return 0;
        }
        else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        }
    }

    if(strncmp(argv[argc-3], "data2png", 8) == 0 || strncmp(argv[argc-3], "png2data", 8) == 0) {
        convert(argv[argc-2], argv[argc-1], argv[argc-3], verbose);
    }
    else printf("Unknown command %s\n", argv[argc-3]);
    return 0;
}