#ifndef CONVERT_H
#define CONVERT_H
#include <stdbool.h>
void convert(char *input, char *output, char *direction);
void data2png(char *input, char *output, bool targetIsFolder);
void png2data(char *input, char *output, bool targetIsFolder);
#endif //CONVERT_H
