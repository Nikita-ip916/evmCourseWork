#include <libmyscomp.hpp>
#include <libmyterm.hpp>

#ifndef MY_BIGCHARS
#define MY_BIGCHARS

class bigChars : public myTerminal {
public:
    int printA(string str);
    int box(int x1, int y1, int x2, int y2);
    int printBigChar(
            int* big,
            int x,
            int y,
            enum colors fg = standard,
            enum colors bg = standard);
    int setBigCharPos(int* big, int x, int y, int value);
    int getBigCharPos(int* big, int x, int y, int* value);
    int bigCharWrite(int* big, int count);
    int bigCharRead(int* big, int needCount, int* count);
};

#endif
