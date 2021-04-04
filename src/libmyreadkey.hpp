#include <libmyscomp.hpp>
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
    struct termios savedAttr;
    struct termios newAttr;

public:
    int readS(char* str);
    int readK(enum keys* key);
    int mytermSave();
    int mytermRestore();
    int mytermRegime(
            int regime,
            int vtime = 0,
            int vmin = 0,
            int echo = 0,
            int sigint = 0);
};

#endif
