#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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
#include "files.h"
#include "types.h"

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

bool writeFile(char *path, unsigned char *data, size_t length) {
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        printf("Failed to open file for writing");
        return false;
    }

    size_t written = fwrite(data, 1, length, file);
    if (written != length) {
        printf("Failed to write all bytes");
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
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