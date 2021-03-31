#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <libmyreadkey.hpp>

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
    int er = 0;
    readKey rk;
    keys key = zeroKey;

    rk.clrscr();
    rk.gotoXY(24, 1);

    rk.mytermRegime(0, 0, 0, 1, 0);
    rk.mytermSave();

    while (1) {
        rk.readK(&key);
        if (key != zeroKey) {
            cout << "Вы нажали на клавишу " << keys(key) << "\n";
            break;
        }
    }
    key = zeroKey;
    rk.mytermRestore();
    while (1) {
        rk.readK(&key);
        if (key != zeroKey) {
            cout << "Вы нажали на клавишу " << keys(key) << "\n";
            break;
        }
    }
    return er;
}
