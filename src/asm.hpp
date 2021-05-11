#ifndef ASM_H
#define ASM_H

#include "src/libmyscomp.hpp"

int myPow(int a, int b);
int sreadInt(char* buffers, int* num, int radix);
int asmTranslator(int argc, char** argv);
int asmCommand(char* str);
int parsingLine(char* str, int* address, int* value);
int strToCommand(char* ptr, int* value);
int checkArgv(char* argv[]);

#endif
