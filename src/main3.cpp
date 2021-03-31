#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <libmybigchars.hpp>
#include <libmyscomp.hpp>
#include <libmyterm.hpp>

using namespace std;

#define error(er)                \
    if (er > 0) {                \
        bc.setfgcolor(standard); \
        bc.setbgcolor(standard); \
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
    bigChars bc;
    int er, tmp, cnt;
    int *bigPair, *bigPair2;
    bigPair = new int[2];
    bigPair2 = new int[2];
    bigPair[0] = 811647231;
    bigPair[1] = 50727960;

    er = bc.clrscr();

    er = bc.bigCharWrite(bigPair, 1);
    er = bc.bigCharRead(bigPair2, 2, &cnt);

    er = bc.gotoXY(3, 2);
    error(er);
    er = bc.printA("aaaffg");

    er = bc.box(10, 5, 19, 14);
    error(er);
    er = bc.printBigChar(bigPair, 11, 6, red, black);
    error(er);
    er = bc.getBigCharPos(bigPair, 2, 3, &tmp);
    error(er);
    er = bc.setBigCharPos(bigPair, 2, 3, 1 - tmp);
    error(er);

    er = bc.printBigChar(bigPair, 11, 6, red, black);
    error(er);
    er = bc.gotoXY(9, 15);
    error(er);
    bc.writeT("Количество считанных символов: " + to_string(cnt));
    er = bc.box(10, 15, 19, 24);
    error(er);
    er = bc.printBigChar(bigPair2, 11, 16, cyan, black);
    error(er);

    er = bc.gotoXY(24, 1);
    error(er);

    return er;
}
