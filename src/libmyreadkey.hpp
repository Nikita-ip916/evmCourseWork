#include <libmyterm.hpp>
#include <string.h>

#ifndef MY_READKEY
#define MY_READKEY

enum keys {
    zeroKey,
    l,
    s,
    r,
    t,
    i,
    f5,
    f6,
    leftKey,
    rightKey,
    upKey,
    downKey,
    enter,
    q
};

class readKey : public myTerminal {
    static struct termios savedAttr;
    static struct termios newAttr;

public:
    static int readS(char* str);
    static int readK(enum keys* key);
    static int mytermSave();
    static int mytermRestore();
    static int mytermRegime(
            int regime,
            int vtime = 0,
            int vmin = 0,
            int echo = 0,
            int sigint = 0);
};

#endif
