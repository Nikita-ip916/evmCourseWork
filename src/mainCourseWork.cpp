#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <libmybigchars.hpp>
#include <libmyreadkey.hpp>
#include <libmyscomp.hpp>
#include <libmyterm.hpp>
#include <sstream>
#include <string.h>

using namespace std;

int termW;
int termH;

#include <interface.hpp>

int main()
{
    srand(time(0));
    mySimpleComputer sc;
    bigChars bc;
    int activeCell = 0;
    int* bigArr = new int[36];
    char fileName[256] = "";
    string stInput = "Введите команду: ";
    string fileNameInput = "Введите имя файла: ";

    sc.memoryInit();
    sc.regInit();

    int cnt;
    bc.bigCharRead(bigArr, 18, &cnt);
    if (cnt < 18) {
        cout << " Потеряны большие символы\n";
        return -1;
    }
    if (bc.gotoXY(30, 84) == -1) {
        cout << " Недостаточный размер терминала, минимум (30x84)\n";
        return -1;
    }
    bc.getscreensize(&termH, &termW);

    bordersUpdate(bc, sc, stInput);

    ////////////////////////////////////////////// Динамические подписи
    regUpdate(bc, sc);

    int el; // проверка заполнением
    for (int i = 0; i < 100; i++) {
        el = rand() % 0x8000;
        sc.memorySet(i, el);
    }

    memUpdate(bc, sc, activeCell, bigArr);

    sc.clearString(stInput);
    bc.setCursInv();
    sc.mytermRegime(0, 0, 0, 0, 1);
    sc.mytermSave();

    signal(SIGALRM, sc.signalHandler);
    signal(SIGUSR1, sc.signalHandler);
    // struct itimerval nval, oval;
    // nval.it_interval.tv_sec = 3;
    // nval.it_interval.tv_usec = 500;
    // nval.it_value.tv_sec = 1;
    // nval.it_value.tv_usec = 0;

    keys key;
    do {
        key = zeroKey;
        sc.readK(&key);
        int ignore;
        sc.regGet(T, &ignore);
        if (ignore) {
            switch (key) {
            case zeroKey:
                break;
            case l: // загрузка ram из файла
                bc.setCursVis();
                sc.mytermRestore();
                do {
                    sc.clearString(fileNameInput);
                    sc.readS(fileName);
                    strcat(fileName, ".dat");
                    bordersUpdate(bc, sc, stInput);
                    memUpdate(bc, sc, activeCell, bigArr);
                    regUpdate(bc, sc);
                } while (strlen(fileName) == 4);
                bc.setCursInv();
                if (sc.memoryLoad(fileName)) {
                    sc.clearString("Файл " + string(fileName) + " не найден");
                    bc.gotoXY(24, 84);
                    sleep(2);
                }
                fileName[256] = {0};
                memUpdate(bc, sc, activeCell, bigArr);
                sc.mytermSave();
                break;
            case s: // сохранение ram в файл
                bc.setCursVis();
                sc.mytermRestore();
                do {
                    sc.clearString(fileNameInput);
                    sc.readS(fileName);
                    strcat(fileName, ".dat");
                    bordersUpdate(bc, sc, stInput);
                    memUpdate(bc, sc, activeCell, bigArr);
                    regUpdate(bc, sc);
                } while (strlen(fileName) == 4);
                bc.setCursInv();
                sc.memorySave(fileName);
                fileName[256] = {0};
                sc.mytermSave();
                break;
            case r: // запустить программу на выполнение ---
                sc.clearString("Input: ");
                sc.clearString("Output: ", 0);
                sc.regSet(T, 0);
                regUpdate(bc, sc);
                if (sc.CU()) {
                    regUpdate(bc, sc);
                    raise(SIGUSR1);
                    memUpdate(bc, sc, activeCell, bigArr);
                    sc.clearString("Выход из программы...");
                    sc.clearString("", 0);
                    sleep(2);
                } else {
                    alarm(1);
                    // setitimer(ITIMER_REAL, &nval, &oval);
                }
                break;
            case t: // выполнить только текущую команду ---
                sc.clearString("Input: ");
                sc.clearString("Output: ", 0);
                int tmp;
                sc.regSet(T, 0);
                regUpdate(bc, sc);
                if (sc.CU()) {
                    regUpdate(bc, sc);
                    raise(SIGUSR1);
                    memUpdate(bc, sc, activeCell, bigArr);
                    sc.clearString("Выход из программы...");
                    sc.clearString("", 0);
                    sleep(2);
                } else {
                    sc.counterGet(&tmp);
                    tmp++;
                    if (tmp > 99)
                        tmp = 0;
                    sc.counterSet(tmp);
                    activeCell = tmp;
                }
                sc.regSet(T, 1);
                bordersUpdate(bc, sc, stInput);
                memUpdate(bc, sc, activeCell, bigArr);
                regUpdate(bc, sc);
                break;
            case i: // сброс памяти и регистров до начальных
                sc.memoryInit();
                sc.regInit();
                memUpdate(bc, sc, activeCell, bigArr);
                regUpdate(bc, sc);
                break;
            case f5: // установление значения аккумулятора
                sc.clearString("Измените значение аккумулятора: ");
                memUpdate(bc, sc, 100, bigArr);
                changeValue(bc, sc, 100, bigArr);
                memUpdate(bc, sc, activeCell, bigArr);
                break;
            case f6: // установление значения счётчика команд
                sc.clearString("Измените значение счётчика команд: ");
                memUpdate(bc, sc, 101, bigArr);
                changeValue(bc, sc, 101, bigArr);
                memUpdate(bc, sc, activeCell, bigArr);
                break;
            case leftKey: // выбор ячейки влево
                if (activeCell % 10 == 0) {
                    activeCell += 9;
                } else {
                    activeCell--;
                }
                memUpdate(bc, sc, activeCell, bigArr);
                break;
            case rightKey: // выбор ячейки вправо
                if (activeCell % 10 == 9) {
                    activeCell -= 9;
                } else {
                    activeCell++;
                }
                memUpdate(bc, sc, activeCell, bigArr);
                break;
            case upKey: // выбор ячейки вверх
                if (activeCell <= 9) {
                    activeCell += 90;
                } else {
                    activeCell -= 10;
                }
                memUpdate(bc, sc, activeCell, bigArr);
                break;
            case downKey: // выбор ячейки вниз
                if (activeCell >= 90) {
                    activeCell -= 90;
                } else {
                    activeCell += 10;
                }
                memUpdate(bc, sc, activeCell, bigArr);
                break;
            case enter:
                sc.clearString("Измените значение ячейки памяти: ");
                changeValue(bc, sc, activeCell, bigArr);
                break;
            case q: // выход из программы
                break;
            }
        } else if (key == i) {
            raise(SIGUSR1);
            memUpdate(bc, sc, activeCell, bigArr);
        } else {
            key = zeroKey;
            pause();
            if (sc.CU()) {
                regUpdate(bc, sc);
                raise(SIGUSR1);
                memUpdate(bc, sc, activeCell, bigArr);
                sc.clearString("Выход из программы...");
                sc.clearString("", 0);
                sleep(2);
            }
            sc.counterGet(&activeCell);
            bordersUpdate(bc, sc, stInput);
            memUpdate(bc, sc, activeCell, bigArr);
            regUpdate(bc, sc);
        }
        if (key != zeroKey && key != r && key != t) {
            sc.clearString(stInput);
            regUpdate(bc, sc);
        }
    } while (key != q);

    bc.setCursVis();
    bc.clrscr();
    sc.mytermRestore();
    return 0;
}
