#include "libmyreadkey.hpp"

int readKey::readS(char* str)
{
    int read_chars;
    char buf[256];
    if ((read_chars = read(fd, buf, 255)) > 0) {
        strncpy(str, buf, read_chars - 1);
        str[read_chars - 1] = '\0';
    }
    return 0;
}

int readKey::readK(enum keys* key)
{
    int read_chars;
    char buf[256];
    if ((read_chars = read(fd, buf, 255)) > 0) {
        switch (buf[0]) {
        case 'l':
            *key = l;
            break;
        case 's':
            *key = s;
            break;
        case 'r':
            *key = r;
            break;
        case 't':
            *key = t;
            break;
        case 'i':
            *key = i;
            break;
        case 'q':
            *key = q;
            break;
        case '\e':
            if (buf[3] == '5' && buf[2] == '1' && buf[1] == '[')
                *key = f5;
            else if (buf[3] == '7' && buf[2] == '1' && buf[1] == '[')
                *key = f6;
            else if (buf[2] == 'D' && buf[1] == '[')
                *key = leftKey;
            else if (buf[2] == 'C' && buf[1] == '[')
                *key = rightKey;
            else if (buf[2] == 'A' && buf[1] == '[')
                *key = upKey;
            else if (buf[2] == 'B' && buf[1] == '[')
                *key = downKey;
            break;
        case '\n':
            *key = enter;
            break;
        }
    }
    return 0;
}
int readKey::mytermSave()
{
    if (tcgetattr(fd, &savedAttr))
        return -1;
    if (tcsetattr(fd, TCSANOW, &newAttr))
        return -1;
    return 0;
}
int readKey::mytermRestore()
{
    if (tcsetattr(fd, TCSANOW, &savedAttr))
        return -1;
    return 0;
}
int readKey::mytermRegime(int regime, int vtime, int vmin, int echo, int sigint)
{
    if (tcgetattr(fd, &newAttr))
        return -1;
    if (regime == 0) {
        newAttr.c_lflag &= ~ICANON;

        if (sigint == 0)
            newAttr.c_lflag &= ~ISIG;
        else
            newAttr.c_lflag |= ISIG;

        if (echo == 0)
            newAttr.c_lflag &= ~ECHO;
        else
            newAttr.c_lflag |= ECHO;

        newAttr.c_cc[VMIN] = vmin;
        newAttr.c_cc[VTIME] = vtime;
    } else
        newAttr.c_lflag |= ICANON;

    return 0;
}
