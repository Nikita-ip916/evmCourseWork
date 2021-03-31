#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <curses.h>
#include <iostream>
#include <libmybigchars.hpp>
#include <libmyscomp.hpp>
#include <libmyterm.hpp>

using namespace std;

#define error(er)                \
    if (er > 0) {                \
        mt.setfgcolor(standard); \
        mt.setbgcolor(standard); \
        errorCatch(er);          \
        return er;               \
    }

void errorCatch(int er)
{
    switch (er) {
    case P:
        cout << "\nПереполнение при выполнении операции\n";
        break;
    case O:
        cout << "\nОшибка деления на нуль\n";
        break;
    case M:
        cout << "\nВыход за границы памяти\n";
        break;
    case T:
        cout << "\nИгнорирование тактовых импульсов\n";
        break;
    case E:
        cout << "\nУказана неверная команда\n";
        break;
    }
}

int main()
{
    myTerminal mt;
    int er;
    int scrW = 0, scrH = 0;
    int x = 15, y = 22;

    er = mt.clrscr();
    for (int i = 1; i <= x; i += 2)
        for (int j = 1; j <= y; j += 3) {
            er = mt.gotoXY(i, j);
            error(er);
            mt.setfgcolor(colors(i / 2));
            mt.setbgcolor(colors(j / 3));
            mt.writeT(to_string(((i / 2 + 1) * (j / 3 + 1)) % 100));
        }
    er = mt.gotoXY(16, 10);
    error(er);
    mt.setfgcolor();
    mt.setbgcolor();
    mt.getscreensize(&scrH, &scrW);

    string buf;
    buf = "Ширина " + to_string(scrW) + ' ' + " Высота " + to_string(scrH);
    mt.writeT(buf);

    er = mt.gotoXY(19, 1);

    return er;
}
