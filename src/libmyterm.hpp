#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sys/ioctl.h>
#include <termios.h>

#ifndef MY_TERMINAL
#define MY_TERMINAL

#define wr1(regFlags, reg) regFlags |= (1 << reg)
#define wr0(regFlags, reg) regFlags &= ~(1 << reg)
#define getFlag(regFlags, reg) (regFlags >> reg) & 1

enum colors {
    cl_black,
    cl_red,
    cl_green,
    cl_yellow,
    cl_blue,
    cl_magenta,
    cl_cyan,
    cl_gray,
    cl_default
};

class myTerminal {
protected:
    static int fd;

public:
    myTerminal()
    {
        fd = 0;
    }
    static void setfd(int);
    static int getfd();

    static int setCursVis();
    static int setCursInv();
    static void writeT(string);

    static int clrscr();
    static int gotoXY(int x, int y);
    static int getscreensize(int* rows, int* cols);
    static int setfgcolor(enum colors c = cl_default);
    static int setbgcolor(enum colors c = cl_default);
};

#endif
