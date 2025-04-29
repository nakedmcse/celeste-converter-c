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
}

void png2data(char *input, char *output, bool targetIsFolder) {
    // Implement png to data
    printf("Converting %s to %s%s\n", input, output, targetIsFolder ? " (folder)" : "");
}