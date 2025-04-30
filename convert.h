#ifndef CONVERT_H
#define CONVERT_H
#include <stdbool.h>
void convert(char *input, char *output, char *direction, bool verbose);
void data2png(char *input, char *output, bool targetIsFolder, bool verbose);
void png2data(char *input, char *output, bool targetIsFolder, bool verbose);
#endif //CONVERT_H
