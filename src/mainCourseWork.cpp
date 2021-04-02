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

#define error(er)                \
    if (er > 0) {                \
        bc.setfgcolor(standard); \
        bc.setbgcolor(standard); \
        sc.regSet(er, 1);        \
    }

void clearInput(bigChars& bc, string strInput)
{
    int tmp = strInput.length() + 1;
    for (int i = strInput.length() + 1; i <= termW; i++) {
        strInput += " ";
    }
    bc.gotoXY(24, 1);
    bc.writeT(strInput);
    bc.gotoXY(24, tmp - 15);
}

void bordersUpdate(bigChars& bc)
{
    bc.clrscr();
    /////////////////////////////////////////////// Рамки
    bc.box(1, 1, 12, 61);
    bc.box(1, 62, 3, 84);
    bc.box(4, 62, 6, 84);
    bc.box(7, 62, 9, 84);
    bc.box(10, 62, 12, 84);
    bc.box(13, 1, 22, 46);
    bc.box(13, 47, 22, 84);

    ///////////////////////////////////////// Статические подписи
    bc.gotoXY(1, 30);
    bc.writeT(" Memory ");
    bc.gotoXY(1, 67);
    bc.writeT(" accumulator ");
    bc.gotoXY(4, 63);
    bc.writeT(" instructionCounter ");
    bc.gotoXY(7, 68);
    bc.writeT(" Operation ");
    bc.gotoXY(10, 70);
    bc.writeT(" Flags ");
    bc.gotoXY(13, 48);
    bc.writeT(" Keys: ");
    bc.gotoXY(14, 48);
    bc.writeT("l - load");
    bc.gotoXY(15, 48);
    bc.writeT("s - save");
    bc.gotoXY(16, 48);
    bc.writeT("r - run");
    bc.gotoXY(17, 48);
    bc.writeT("t - step");
    bc.gotoXY(18, 48);
    bc.writeT("i - reset");
    bc.gotoXY(19, 48);
    bc.writeT("F5 - accumulator");
    bc.gotoXY(20, 48);
    bc.writeT("F6 - instructionCounter");
    bc.gotoXY(24, 1);
    bc.writeT("Input\\Output: ");
}

void regUpdate(bigChars& bc, mySimpleComputer& sc)
{
    int checkReg = 0;
    string regStr = "";
    bc.gotoXY(11, 69);
    sc.regGet(P, &checkReg);
    if (checkReg)
        regStr += " P";
    sc.regGet(O, &checkReg);
    if (checkReg)
        regStr += " O";
    sc.regGet(M, &checkReg);
    if (checkReg)
        regStr += " M";
    sc.regGet(T, &checkReg);
    if (checkReg)
        regStr += " T";
    sc.regGet(E, &checkReg);
    if (checkReg)
        regStr += " E";
    bc.writeT(regStr);
}

void memUpdate(bigChars& bc, mySimpleComputer& sc, int activeCell, int* bigArr)
{
    int el, er;
    int command = 0, operand = 0;
    for (int i = 0; i < n; i++) {
        int y = i / 10 + 2;
        int x = (i % 10) * 6 + 2;
        er = sc.memoryGet(i, &el);
        error(er);

        er = sc.commandDecode(el, &command, &operand);
        error(er);

        bc.gotoXY(y, x);

        string value;
        value = "+";
        ostringstream s;
        s << setfill('0') << setw(2) << hex << command << setfill('0')
          << setw(2) << hex << operand;
        value += s.str();
        if (i == activeCell) {
            int* bigPair = new int[2];
            bigPair[0] = bigArr[32];
            bigPair[1] = bigArr[33];
            bc.printBigChar(bigPair, 14, 2);
            for (int j = 1; j < 3; j++) {
                bigPair[0] = bigArr[2 * (command >> (4 * (2 - j)) & 15)];
                bigPair[1] = bigArr[2 * (command >> (4 * (2 - j)) & 15) + 1];
                bc.printBigChar(bigPair, 14, 2 + j * 9);
            }
            for (int j = 3; j < 5; j++) {
                bigPair[0] = bigArr[2 * (operand >> (4 * (4 - j)) & 15)];
                bigPair[1] = bigArr[2 * (operand >> (4 * (4 - j)) & 15) + 1];
                bc.printBigChar(bigPair, 14, 2 + j * 9);
            }
            bc.gotoXY(y, x);
            bc.setbgcolor(gray);
            bc.setfgcolor(black);
        }
        bc.writeT(value);
        bc.setbgcolor();
        bc.setfgcolor();
    }
}

int main()
{
    mySimpleComputer sc;
    bigChars bc;
    readKey rk;
    int er;
    int activeCell = 0;
    int* bigArr = new int[36];
    char fileName[256] = "";
    string fileNameInput = "Введите имя файла: ";

    sc.memoryInit();
    sc.regInit();

    int cnt;
    bc.bigCharRead(bigArr, 18, &cnt);
    if (cnt < 18) {
        cout << "Потеряны большие символы\n";
        return -1;
    }
    if (bc.gotoXY(24, 84) == M) {
        cout << "Недостаточный размер терминала, минимум (24x84)\n";
        return -1;
    }
    bc.getscreensize(&termH, &termW);

    bordersUpdate(bc);

    ////////////////////////////////////////////// Динамические подписи
    regUpdate(bc, sc);

    int el; // проверка заполнением
    for (int i = 0; i < 100; i++) {
        el = 0;
        er = sc.commandEncode(10, i, &el);
        error(er);
        sc.memorySet(i, el);
    }

    memUpdate(bc, sc, activeCell, bigArr);

    bc.gotoXY(24, 15);
    rk.mytermRegime(0, 0, 0, 0, 1);
    rk.mytermSave();

    keys key;
    do {
        key = zeroKey;
        rk.readK(&key);
        switch (key) {
        case zeroKey:
            break;
        case l: // загрузка ram из файла
            rk.mytermRestore();
            do {
                clearInput(bc, fileNameInput);
                rk.readS(fileName);
                strcat(fileName, ".dat");
                bordersUpdate(bc);
                memUpdate(bc, sc, activeCell, bigArr);
                regUpdate(bc, sc);
            } while (strlen(fileName) == 4);
            er = sc.memoryLoad(fileName);
            error(er);
            memUpdate(bc, sc, activeCell, bigArr);
            rk.mytermSave();
            break;
        case s: // сохранение ram в файл
            rk.mytermRestore();
            do {
                clearInput(bc, fileNameInput);
                rk.readS(fileName);
                strcat(fileName, ".dat");
                bordersUpdate(bc);
                memUpdate(bc, sc, activeCell, bigArr);
                regUpdate(bc, sc);
            } while (strlen(fileName) == 4);
            sc.memorySave(fileName);
            rk.mytermSave();
            break;
        case r: // запустить программу на выполнение ---
            break;
        case t: // выполнить только текущую команду ---
            break;
        case i: // сброс памяти и регистров до начальных
            sc.memoryInit();
            sc.regInit();
            memUpdate(bc, sc, activeCell, bigArr);
            regUpdate(bc, sc);
            break;
        case f5: // установление значения аккумулятора ---
            break;
        case f6: // установление значения счётчика команд ---
            break;
        case leftKey: // выбор ячейки влево ??
            if (activeCell % 10 == 0) {
                activeCell += 9;
            } else {
                activeCell--;
            }
            memUpdate(bc, sc, activeCell, bigArr);
            break;
        case rightKey: // выбор ячейки вправо ??
            if (activeCell % 10 == 9) {
                activeCell -= 9;
            } else {
                activeCell++;
            }
            memUpdate(bc, sc, activeCell, bigArr);
            break;
        case upKey: // выбор ячейки вверх ??
            if (activeCell <= 9) {
                activeCell += 90;
            } else {
                activeCell -= 10;
            }
            memUpdate(bc, sc, activeCell, bigArr);
            break;
        case downKey: // выбор ячейки вниз ??
            if (activeCell >= 90) {
                activeCell -= 90;
            } else {
                activeCell += 10;
            }
            memUpdate(bc, sc, activeCell, bigArr);
            break;
        case q: // выход из программы
            break;
        }
        if (key != zeroKey) {
            regUpdate(bc, sc);
            bc.gotoXY(24, 15);
        }
    } while (key != q);
    bc.clrscr();
    rk.mytermRestore();
    return 0;
}
