#ifndef FILES_H
#define FILES_H
#include "types.h"
bool isFolder(char *path);
void getFiles(char *path, StringArray *files);
unsigned char *readFile(char *path);
bool writeFile(char *path, unsigned char *data, size_t length);
char* getFilename(char* path);
void setExtension(char *path, char *ext);
#endif //FILES_H
