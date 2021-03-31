#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <libmyscomp.hpp>

using namespace std;

#define error(er)       \
    if (er > 0) {       \
        errorCatch(er); \
        return er;      \
    }

void errorCatch(int er)
{
    switch (er) {
    case P:
        cout << "Переполнение при выполнении операции\n";
        break;
    case O:
        cout << "Ошибка деления на нуль\n";
        break;
    case M:
        cout << "Выход за границы памяти\n";
        break;
    case T:
        cout << "Игнорирование тактовых импульсов\n";
        break;
    case E:
        cout << "Указана неверная команда\n";
        break;
    }
}
int main()
{
    srand(time(0));
    char fileName[256] = "memory.dat";
    int er = 0;
    mySimpleComputer sc;
    int el;
    er = sc.memoryInit();
    for (int i = 0; i < n; i++) {
        er = sc.memorySet(i, i + 1);
        error(er);
    }

    for (int i = 0; i < n; i++) {
        er = sc.memoryGet(i, &el);
        error(er);
        cout << el << " ";
    }
    cout << "\n";

    er = sc.memorySave(fileName);
    error(er);

    for (int i = 0; i < n; i++) {
        er = sc.memorySet(i, rand() % 101);
        error(er);
    }
    for (int i = 0; i < n; i++) {
        er = sc.memoryGet(i, &el);
        error(er);
        cout << el << " ";
    }
    cout << "\n";

    er = sc.memoryLoad(fileName);
    error(er);

    for (int i = 0; i < n; i++) {
        er = sc.memoryGet(i, &el);
        error(er);
        cout << el << " ";
    }
    cout << "\n\n";

    er = sc.regInit();
    er = sc.regSet(1, 1);
    er = sc.regSet(4, 1);
    er = sc.regSet(5, 1);
    error(er);
    for (int i = 1; i <= int(sizeof(int) * 8); i++) {
        er = sc.regGet(i, &el);
        error(er);
        cout << el << " ";
    }
    cout << "\n\n";

    int command, operand;
    cin >> command;
    cin >> operand;
    er = sc.commandEncode(command, operand, &el);
    error(er);
    cout << " " << el << "\n";
    er = sc.commandDecode(el, &command, &operand);
    error(er);
    cout << command << " " << operand << "\n";
}
