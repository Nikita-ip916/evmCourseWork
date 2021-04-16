#include "libmyterm.hpp"

// clear_screen=\E[H\E[J
// cursor_address=\E[%i%p1%d;%p2%dH
// set_a_background=\E[4%p1%dm
// set_a_foreground=\E[3%p1%dm
// cursor_invisible=\E[?25l\E[?1c
// cursor_visible=\E[?25h\E[?8c

// echo
// -e"\E[H\E[J\E[5;10H\E[31m\E[40mНикитаЧигвинцев\E[6;8H\E[32m\E[47mИП-916\E[10;1H\E[m"
int myTerminal::getfd()
{
    return fd;
}
void myTerminal::setfd(int Fd)
{
    fd = Fd;
}
int myTerminal::setCursVis()
{
    char buf[] = "\E[?25h\E[?8c";
    write(fd, buf, 12);
    return 0;
}
int myTerminal::setCursInv()
{
    char buf[] = "\E[?25l\E[?1c";
    write(fd, buf, 12);
    return 0;
}
void myTerminal::writeT(string buf)
{
    write(fd, buf.c_str(), int(buf.length() + 1));
}
int myTerminal::clrscr()
{
    char buf[] = "\E[H\E[J";
    write(fd, buf, 7);
    return 0;
}
int myTerminal::gotoXY(int x, int y)
{
    int rows, cols, er;
    er = getscreensize(&rows, &cols);
    if (x > 0 && x <= rows && y > 0 && y <= cols) {
        string buf = "\E[" + to_string(x) + ';' + to_string(y) + 'H';
        write(fd, buf.c_str(), int(buf.length() + 1));
        return er;
    }
    return -1;
}
int myTerminal::getscreensize(int* rows, int* cols)
{
    struct winsize ws;

    if (!ioctl(1, TIOCGWINSZ, &ws)) {
        *rows = ws.ws_row;
        *cols = ws.ws_col;
    } else {
        fprintf(stderr, "\nОшибка получения размера экрана.\n");
        return -1;
    }
    return 0;
}
int myTerminal::setfgcolor(enum colors c)
{
    string buf;
    if (c == 8)
        buf = "\E[39m";
    else
        buf = "\E[3" + to_string(c) + 'm';
    write(fd, buf.c_str(), 6);
    return 0;
}
int myTerminal::setbgcolor(enum colors c)
{
    string buf;
    if (c == 8)
        buf = "\E[49m";
    else
        buf = "\E[4" + to_string(c) + 'm';
    write(fd, buf.c_str(), 6);
    return 0;
}
