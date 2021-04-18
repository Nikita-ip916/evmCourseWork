#include <libmyterm.hpp>

#ifndef MY_BIGCHARS
#define MY_BIGCHARS

class bigChars : public myTerminal {
public:
    static int printA(string str);
    static int box(int x1, int y1, int x2, int y2);
    static int printBigChar(
            int* big,
            int x,
            int y,
            enum colors fg = cl_default,
            enum colors bg = cl_default);
    static int setBigCharPos(int* big, int x, int y, int value);
    static int getBigCharPos(int* big, int x, int y, int* value);
    static int bigCharWrite(int* big, int count);
    static int bigCharRead(int* big, int needCount, int* count);
};

#endif
