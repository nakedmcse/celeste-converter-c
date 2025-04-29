#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
    #include <sys/types.h>
    #include <sys/stat.h>
    #define stat _stat
#else
    #include <sys/stat.h>
    #include <unistd.h>
#endif

#include "lodepng.h"
#include "convert.h"

void data2png(char *input, char *output) {
    // Implement data to png
    // Determine if file -> file
    // folder -> folder
    // file -> folder
}

void png2data(char *input, char *output) {
    // Implement png to data
    // Determine if file -> file
    // folder -> folder
    // file -> folder
}

bool isFolder(char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return false;  // stat failed
    }
    return (path_stat.st_mode & S_IFMT) == S_IFDIR;
}