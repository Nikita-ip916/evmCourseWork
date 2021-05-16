#include "asm.hpp"
#include "bsc.hpp"

int main(int argc, char** argv)
{
    // printf("%d %s %s\n", argc, argv[1], argv[2]);
    char src[256];
    char dest[256];
    strcpy(src, argv[1]);
    strcpy(dest, argv[2]);
    char* ptr = strchr(dest, '.');
    ptr[1] = 's';
    ptr[2] = 'a';
    ptr[3] = '\0';
    char* argp[3] = {NULL, src, dest};
    // printf("%d %s %s\n", argc, argp[1], argp[2]);
    if (basicTranslator(argc, argp))
        return 1;
    ptr = strchr(src, '.');
    ptr[1] = 's';
    ptr[2] = 'a';
    ptr[3] = '\0';
    ptr = strchr(dest, '.');
    ptr[1] = 'o';
    ptr[2] = '\0';
    argp[1] = src;
    argp[2] = dest;
    // printf("%d %s %s\n", argc, src, dest);
    return asmTranslator(argc, argp);
}
