#include "libmybigchars.hpp"

int bigChars::printA(string str)
{
    string buf;
    buf += "\E)0\016" + str + "\017";
    write(fd, buf.c_str(), int(buf.length() + 1));
    return 0;
}
int bigChars::box(int x1, int y1, int x2, int y2)
{
    string buf;
    int er;
    buf = "\E)0\016";
    write(fd, buf.c_str(), int(buf.length() + 1));
    if (er = gotoXY(x2, y2))
        return -1;
    if (er = gotoXY(x1, y1))
        return -1;
    for (int i = x1; i <= x2; i++) {
        for (int j = y1; j <= y2; j++) {
            er = gotoXY(i, j);
            if (i == x1 && j == y1)
                buf = "l";
            else if (i == x1 && j == y2)
                buf = "k";
            else if (i == x2 && j == y1)
                buf = "m";
            else if (i == x2 && j == y2)
                buf = "j";
            else if (i == x1 || i == x2)
                buf = "q";
            else if (j == y1 || j == y2)
                buf = "x";
            else
                buf = "";
            write(fd, buf.c_str(), int(buf.length() + 1));
        }
    }
    buf = "\017";
    write(fd, buf.c_str(), int(buf.length() + 1));
    return 0;
}
int bigChars::printBigChar(
        int* big, int x, int y, enum colors fg, enum colors bg)
{
    string buf;
    int er, n = 8;

    setfgcolor(fg);
    setbgcolor(bg);
    buf = "\E)0\016";
    write(fd, buf.c_str(), int(buf.length() + 1));

    if (er = gotoXY(x, y))
        return -1;
    if (er = gotoXY(x + n - 1, y + n - 1))
        return -1;

    for (int i = 1; i <= n / 2; i++) {
        for (int j = 1; j <= n; j++) {
            er = gotoXY(i + x - 1, j + y - 1);
            if (big[0] >> ((i - 1) * 8 + j - 1) & 1)
                buf = "a";
            else
                buf = " ";
            write(fd, buf.c_str(), int(buf.length() + 1));
        }
    }
    for (int i = n / 2 + 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            er = gotoXY(i + x - 1, j + y - 1);
            if (big[1] >> ((i - 1) * 8 + j - 1) & 1)
                buf = "a";
            else
                buf = " ";
            write(fd, buf.c_str(), int(buf.length() + 1));
        }
    }

    buf = "\017";
    write(fd, buf.c_str(), int(buf.length() + 1));
    setfgcolor();
    setbgcolor();

    return 0;
}
int bigChars::setBigCharPos(int* big, int x, int y, int value)
{
    if (y >= 1 && y <= 8)
        if (value == 0 && 1 <= x && x <= 4)
            wr0(big[0], ((x - 1) * 8 + y - 1));
        else if (value == 0 && 5 <= x && x <= 8)
            wr0(big[1], ((x - 1) * 8 + y - 1));
        else if (value == 1 && 1 <= x && x <= 4)
            wr1(big[0], ((x - 1) * 8 + y - 1));
        else if (value == 1 && 5 <= x && x <= 8)
            wr1(big[1], ((x - 1) * 8 + y - 1));
        else
            return -1;
    else
        return -1;
    return 0;
}
int bigChars::getBigCharPos(int* big, int x, int y, int* value)
{
    if (y >= 1 && y <= 8)
        if (1 <= x && x <= 4)
            *value = getFlag(big[0], ((x - 1) * 8 + y - 1));
        else if (5 <= x && x <= 8)
            *value = getFlag(big[1], ((x - 1) * 8 + y - 1));
        else
            return -1;
    else
        return -1;
    return 0;
}
int bigChars::bigCharWrite(int* big, int count)
{
    FILE* fp;
    fp = fopen("bcData.dat", "wb");

    for (int i = 0; i < count; i++) {
        fwrite(&big[2 * i], sizeof(int), 1, fp);
        fwrite(&big[2 * i + 1], sizeof(int), 1, fp);
    }

    fclose(fp);
    return 0;
}
int bigChars::bigCharRead(int* big, int needCount, int* count)
{
    FILE* fp;
    fp = fopen("bcData.dat", "rb");
    if (!fp) {
        return -1;
    }
    *count = 0;
    for (int i = 0; i < needCount; i++) {
        if (fread(&big[2 * i], sizeof(int), 1, fp)
            && fread(&big[2 * i + 1], sizeof(int), 1, fp))
            (*count)++;
        else
            break;
    }

    fclose(fp);
    return 0;
}
