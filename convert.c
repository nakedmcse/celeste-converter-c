#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
    #include <sys/types.h>
    #include <sys/stat.h>
    #define stat _stat
    #include <windows.h>
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <dirent.h>
#endif

#include "lodepng.h"
#include "convert.h"

// Dynamic String Array
typedef struct StringArray {
    char **strings;
    size_t count;
    size_t capacity;
} StringArray;

void appendString(StringArray *array, char *string) {
    if (array->count >= array->capacity) {
        if (array->capacity == 0) {
            array->capacity = 256;
            array->strings = malloc(array->capacity * sizeof(char *));
        }
        else {
            array->capacity *= 2;
            array->strings = realloc(array->strings, array->capacity * sizeof(char *));
        }
    }
    array->strings[array->count++] = string;
}

// Convert functions
bool isFolder(char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return false;  // stat failed
    }
    return (path_stat.st_mode & S_IFMT) == S_IFDIR;
}

void getFiles(char *path, StringArray *files) {
    if (!isFolder(path)) {
        appendString(files, path);
        return;
    }

#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char search_path[MAX_PATH];

    // Create search pattern: path\*
    snprintf(search_path, sizeof(search_path), "%s\\*", path);

    hFind = FindFirstFile(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Failed to open directory: %s\n", path);
        return;
    }

    do {
        const char *name = find_data.cFileName;
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
            char *target = malloc(strlen(name) + strlen(path) + 2);
            sprintf(target, "%s\\%s", path, name);
            appendString(files, target);
        }
    } while (FindNextFile(hFind, &find_data) != 0);

    FindClose(hFind);

#else
    struct dirent *entry;

    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("Failed to open directory: %s\n", path);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        const char *name = entry->d_name;
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
            char *target = malloc(strlen(name) + strlen(path) + 2);
            sprintf(target, "%s/%s", path, name);
            appendString(files, target);
        }
    }

    closedir(dir);
#endif
}

unsigned char *readFile(char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        printf("Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    unsigned char *data = malloc(size);
    if (!data) {
        printf("Failed to allocate file memory\n");
        return NULL;
    }

    size_t bytes_read = fread(data, 1, size, file);
    if (bytes_read != size) {
        printf("Failed to read file: %s\n", path);
        free(data);
        return NULL;
    }
    fclose(file);
    return data;
}

char* getFilename(char* path) {
    char *slash1 = strrchr(path, '/');
    char *slash2 = strrchr(path, '\\');
    char *slash = slash1 > slash2 ? slash1 : slash2;
    return slash ? slash + 1 : path; // If found, return after slash; otherwise, whole path is filename
}

void setExtension(char *path, char *ext) {
    char *last_dot = strrchr(path, '.');

    if (last_dot) *last_dot = '\0'; // terminate the string at the dot

    strcat(path, ".");
    strcat(path, ext);
}

uint8_t getPixelChannel(unsigned char *png, uint32_t x, uint32_t y, uint32_t width, uint32_t channel, bool hasAlphe) {
    uint32_t bytesPerPixel = hasAlphe ? 4 : 3;
    uint32_t offset = ((y * width) + x) * bytesPerPixel;
    return png[offset+channel];
}

void convert(char *input, char *output, char *direction) {
    StringArray workfiles;
    workfiles.count = 0;
    workfiles.capacity = 0;
    bool targetIsFolder = isFolder(output);

    getFiles(input, &workfiles);

    if (workfiles.count == 0) return;
    if (workfiles.count > 1 && !targetIsFolder) {
        printf("More than one input file and output is not a folder\n");
        return;
    }

    for (int i = 0; i < workfiles.count; i++) {
        if (strncmp(direction, "data2png", 8) == 0) data2png(workfiles.strings[i], output, targetIsFolder);
        else png2data(workfiles.strings[i], output, targetIsFolder);
    }
}

void data2png(char *input, char *output, bool targetIsFolder) {
    // Implement data to png
    printf("Converting %s to %s%s\n", input, output, targetIsFolder ? " (folder)" : "");

    // Read data file into memory
    unsigned char *inputFile = readFile(input);
    if (inputFile == NULL) return;
    uint32_t width;
    memcpy(&width, inputFile, sizeof(uint32_t));
    uint32_t height;
    memcpy(&height, inputFile + 4, sizeof(uint32_t));
    bool hasAlpha = (inputFile[8] != 0);
    printf("Width: %u, Height: %u, Alpha: %d\n", width, height, hasAlpha);

    // Allocate memory for png
    unsigned char *outputPng = malloc(width * height * 4);
    if (!outputPng) {
        printf("Failed to allocate output png\n");
        return;
    }

    // Process data file
    int index = 0;
    int offset = 9;
    int pngPos = 0;
    while (index < width * height) {
        // Read the RLE count
        uint8_t count = inputFile[offset++];
        if (count == 0) {
            printf("RLE count corrupt\n");
            return;
        }
        // Read RGB(A) channel values
        uint8_t a, r, g, b;
        if (hasAlpha) {
            a = inputFile[offset++];
            if (a == 0) {
                r = 0;
                b = 0;
                g = 0;
            }
            else {
                b = inputFile[offset++];
                g = inputFile[offset++];
                r = inputFile[offset++];
            }
        }
        else {
            b = inputFile[offset++];
            g = inputFile[offset++];
            r = inputFile[offset++];
            a = 0;
        }

        // Output RLE span of same color pixels
        for (int i = 0; i < count; i++) {
            outputPng[pngPos++] = r;
            outputPng[pngPos++] = g;
            outputPng[pngPos++] = b;
            if (hasAlpha) outputPng[pngPos++] = a;
        }

        // Increment index by RLE count
        index += count;
    }

    // Write png to disk
    char *inputFileName = getFilename(input);
    setExtension(inputFileName, "png");
    char *outputFileName = malloc(strlen(output)+strlen(inputFileName)+3);
    strncpy(outputFileName, output, strlen(output));
    if (targetIsFolder) {
        #ifdef _WIN32
        outputFileName = strcat(outputFileName, "\\");
        #else
        outputFileName = strcat(outputFileName, "/");
        #endif
        outputFileName = strcat(outputFileName, inputFileName);
    }
    lodepng_encode_file(outputFileName, outputPng, width, height, hasAlpha ? LCT_RGBA : LCT_RGB, 8);

    // Free png and data
    free(inputFile);
    free(outputPng);
}

void png2data(char *input, char *output, bool targetIsFolder) {
    // Implement png to data
    printf("Converting %s to %s%s\n", input, output, targetIsFolder ? " (folder)" : "");

    // Read png file into memory
    unsigned char* image = 0;
    unsigned width, height;
    unsigned char* png = 0;
    size_t pngsize;
    LodePNGState state;

    lodepng_state_init(&state);
    unsigned error = lodepng_load_file(&png, &pngsize, input);
    if(!error) error = lodepng_decode(&image, &width, &height, &state, png, pngsize);
    if(error) {
        printf("PNG decode error %u: %s\n", error, lodepng_error_text(error));
        return;
    }
    bool hasAlpha = lodepng_is_alpha_type(&state.info_png.color);
    printf("Width: %u, Height: %u, Alpha: %d\n", width, height, hasAlpha);

    // Allocate memory for data
    unsigned char *outputData = malloc(width * height * 4);
    if (!outputData) {
        printf("Failed to allocate output data\n");
        return;
    }

    // Process png file
    memcpy(outputData, &width, sizeof(uint32_t));
    memcpy(outputData+4, &height, sizeof(uint32_t));
    outputData[8] = hasAlpha;

    // Write data to disk
    uint32_t index = 0;
    uint32_t offset = 9;
    while (index < width * height) {
        // Get colors of current pixel
        uint32_t x = index % width;
        uint32_t y = index / width;
        uint8_t r = getPixelChannel(image, x, y, width, 0, hasAlpha);
        uint8_t g = getPixelChannel(image, x, y, width, 1, hasAlpha);
        uint8_t b = getPixelChannel(image, x, y, width, 2, hasAlpha);
        uint8_t a = hasAlpha ? getPixelChannel(image, x, y, width, 3, hasAlpha) : 0;

        // Look ahead at next 255 pixels for RLE
        uint32_t count = 1;
        while (count < 255) {
            if (index + count > width * height) break;  // out of bounds
            uint32_t x2 = (index + count) % width;
            uint32_t y2 = (index + count) / width;
            uint8_t r2 = getPixelChannel(image, x2, y2, width, 0, hasAlpha);
            uint8_t g2 = getPixelChannel(image, x2, y2, width, 1, hasAlpha);
            uint8_t b2 = getPixelChannel(image, x2, y2, width, 2, hasAlpha);
            uint8_t a2 = hasAlpha ? getPixelChannel(image, x2, y2, width, 3, hasAlpha) : 0;
            if (r != r2 || g != g2 || b != b2 || a != a2) break;  // hit end of color run
            count++;
        }
    }

    // Free png and data
    free(outputData);
    lodepng_state_cleanup(&state);
    free(image);
}