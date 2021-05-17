#ifndef BSC_HPP
#define BSC_HPP

#include "asm.hpp"
#include <malloc.h>

enum keywords {
    KEYW_REM = 1,
    KEYW_INPUT = 2,
    KEYW_OUTPUT = 3,
    KEYW_GOTO = 4,
    KEYW_IF = 5,
    KEYW_LET = 6,
    KEYW_END = 7,
    KEYW_E = -1
};

struct tOP {
    struct tOP* next;
    char dt;
};

typedef struct varBasic {
    char name[2];
    int cell;
    int variable;
} var;

typedef struct memoryMap {
    int real;
    int expect;
} memMap;

int swriteInt(char* buff, int num, int radix, int znac);
int keywordCode(char* str);
int parsingLineB(char* str, int output);
int basicTranslator(int argc, char* argv[]);
int ifoperation(int output, char* op, int let, int zn);
void convert(char* str, char* out);
int prior(char c);

#endif
