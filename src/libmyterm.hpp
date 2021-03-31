#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <libmyscomp.hpp>
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

enum colors { black, red, green, yellow, blue, magenta, cyan, gray, standard };

class myTerminal {
protected:
    int fd;

public:
    myTerminal()
    {
        fd = 0;
    }
    void setfd(int);
    int getfd();
    void writeT(string);

    int clrscr();
    int gotoXY(int x, int y);
    int getscreensize(int* rows, int* cols);
    int setfgcolor(enum colors c = standard);
    int setbgcolor(enum colors c = standard);
};

#endif
