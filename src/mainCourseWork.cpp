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

#define POS_ZERO 0x6000
#define NEG_ZERO 0x4000

#define POS_MAX 0x7FFF
#define NEG_MAX 0x5FFF

#define error(er)         \
    if (er > 0) {         \
        sc.regSet(er, 1); \
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

void bordersUpdate(bigChars& bc, string inOut)
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
    clearInput(bc, inOut);
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

void bigCharUpdate(
        int command, int operand, bigChars& bc, int* bigArr, int active = -1)
{
    int* bigPair = new int[2];
    bigPair[0] = bigArr[32];
    bigPair[1] = bigArr[33];
    if (active == 0)
        bc.printBigChar(bigPair, 14, 2, cl_black, cl_gray);
    else
        bc.printBigChar(bigPair, 14, 2);
    for (int j = 1; j < 3; j++) {
        bigPair[0] = bigArr[2 * (command >> (4 * (2 - j)) & 15)];
        bigPair[1] = bigArr[2 * (command >> (4 * (2 - j)) & 15) + 1];
        if (active == 1)
            bc.printBigChar(bigPair, 14, 2 + j * 9, cl_black, cl_gray);
        else
            bc.printBigChar(bigPair, 14, 2 + j * 9);
    }
    for (int j = 3; j < 5; j++) {
        bigPair[0] = bigArr[2 * (operand >> (4 * (4 - j)) & 15)];
        bigPair[1] = bigArr[2 * (operand >> (4 * (4 - j)) & 15) + 1];
        if (active == 2)
            bc.printBigChar(bigPair, 14, 2 + j * 9, cl_black, cl_gray);
        else
            bc.printBigChar(bigPair, 14, 2 + j * 9);
    }
    if (active != -1)
        clearInput(bc, "Измените значение ячейки памяти: ");
}

void bigCharUpdate(int data, bigChars& bc, int* bigArr, int active = -1)
{
    int* bigPair = new int[2];
    if (getFlag(data, 13)) {
        bigPair[0] = bigArr[34];
        bigPair[1] = bigArr[35];
    } else {
        bigPair[0] = 0;
        bigPair[1] = 0;
    }
    if (active == 0)
        bc.printBigChar(bigPair, 14, 2, cl_black, cl_gray);
    else
        bc.printBigChar(bigPair, 14, 2);
    for (int j = 1; j < 5; j++) {
        bigPair[0] = bigArr[2 * ((data & 0x1FFF) >> (4 * (4 - j)) & 15)];
        bigPair[1] = bigArr[2 * ((data & 0x1FFF) >> (4 * (4 - j)) & 15) + 1];
        if ((active == 1 && j < 3) || (active == 2 && j >= 3))
            bc.printBigChar(bigPair, 14, 2 + j * 9, cl_black, cl_gray);
        else
            bc.printBigChar(bigPair, 14, 2 + j * 9);
    }
    if (active != -1)
        clearInput(bc, "Измените значение ячейки памяти: ");
}

void memUpdate(bigChars& bc, mySimpleComputer& sc, int activeCell, int* bigArr)
{
    int el, isData;
    int command = 0, operand = 0;
    for (int i = 0; i < n; i++) {
        int y = i / 10 + 2;
        int x = (i % 10) * 6 + 2;
        sc.memoryGet(i, &el);

        isData = sc.commandDecode(el, &command, &operand);

        bc.gotoXY(y, x);

        string value;
        ostringstream s;
        if (!isData) {
            value = "+";
            s << setfill('0') << setw(2) << hex << command << setfill('0')
              << setw(2) << hex << operand;
        } else {
            if (getFlag(el, 13))
                value = "-";
            else
                value = " ";
            s << setfill('0') << setw(4) << hex << (el & 0x1FFF);
        }
        value += s.str();
        if (i == activeCell) {
            if (!isData)
                bigCharUpdate(command, operand, bc, bigArr);
            else
                bigCharUpdate(el, bc, bigArr);
            bc.gotoXY(y, x);
            bc.setbgcolor(cl_gray);
            bc.setfgcolor(cl_black);
        }
        bc.writeT(value);
        bc.setbgcolor();
        bc.setfgcolor();
    }
}

void changeValue(
        bigChars& bc,
        mySimpleComputer& sc,
        readKey& rk,
        int activeCell,
        int* bigArr)
{
    if (activeCell == 100) {
        // accumulator
    } else if (activeCell == 101) {
        // instructionCounter
    } else if (activeCell >= 0 && activeCell <= 99) {
        int el, isData;
        int command = 0, operand = 0;
        sc.memoryGet(activeCell, &el);
        isData = sc.commandDecode(el, &command, &operand);
        int activeBigChar = 0;
        if (!isData)
            bigCharUpdate(command, operand, bc, bigArr, activeBigChar);
        else
            bigCharUpdate(el, bc, bigArr, activeBigChar);
        keys key;
        do {
            key = zeroKey;
            rk.readK(&key);
            if (key == leftKey) {
                activeBigChar--;
                if (activeBigChar < 0)
                    activeBigChar = 2;
            } else if (key == rightKey) {
                activeBigChar++;
                if (activeBigChar > 2)
                    activeBigChar = 0;
            } else if (key == upKey) {
                if (activeBigChar == 0) {
                    if (getFlag(el, 14)) {
                        if (!getFlag(el, 13)) {
                            isData = 0;
                            wr0(el, 14);
                            sc.commandDecode(el, &command, &operand);
                        } else {
                            wr0(el, 13);
                        }
                    } else {
                        isData = 1;
                        sc.commandEncode(command, operand, &el);
                        wr1(el, 13);
                        wr1(el, 14);
                    }
                } else if (activeBigChar == 1) {
                    if (!isData) {
                        command++;
                        if (command > 127)
                            command = 0;
                    } else {
                        if ((el & !(0xFF)) == NEG_MAX)
                            el = NEG_ZERO + (el & 0xFF);
                        else if ((el & !(0xFF)) == POS_MAX)
                            el = POS_ZERO + (el & 0xFF);
                        else
                            el += (1 << 8);
                    }
                } else if (activeBigChar == 2) {
                    if (!isData) {
                        operand++;
                        if (operand > 127)
                            operand = 0;
                    } else {
                        if (el == NEG_MAX)
                            el = NEG_ZERO;
                        else if (el == POS_MAX)
                            el = POS_ZERO;
                        else
                            el++;
                    }
                }
            } else if (key == downKey) {
                if (activeBigChar == 0) {
                    if (getFlag(el, 14)) {
                        if (!getFlag(el, 13)) {
                            wr1(el, 13);
                        } else {
                            isData = 0;
                            wr0(el, 14);
                            sc.commandDecode(el, &command, &operand);
                        }
                    } else {
                        isData = 1;
                        sc.commandEncode(command, operand, &el);
                        wr0(el, 13);
                        wr1(el, 14);
                    }
                } else if (activeBigChar == 1) {
                    if (!isData) {
                        command--;
                        if (command < 0)
                            command = 127;
                    } else {
                        if ((el & !(0xFF)) == NEG_ZERO)
                            el = (NEG_MAX & !(0xFF)) + (el & 0xFF);
                        else if ((el & !(0xFF)) == POS_ZERO)
                            el = (POS_MAX & !(0xFF)) + (el & 0xFF);
                        else
                            el -= (1 << 8);
                        if (el < 0)
                            el += 8191;
                    }
                } else if (activeBigChar == 2) {
                    if (!isData) {
                        operand--;
                        if (operand < 0)
                            operand = 127;
                    } else {
                        if (el == NEG_ZERO)
                            el = NEG_MAX;
                        else if (el == POS_ZERO)
                            el = POS_MAX;
                        else
                            el--;
                    }
                }
            }
            if (key == enter) {
                if (!isData)
                    sc.commandEncode(command, operand, &el);
                sc.memorySet(activeCell, el);
            }
            if (key != zeroKey) {
                // regUpdate(bc, sc);
                if (!isData) {
                    bigCharUpdate(command, operand, bc, bigArr, activeBigChar);
                } else
                    bigCharUpdate(el, bc, bigArr, activeBigChar);
            }
        } while (key != enter);
        memUpdate(bc, sc, activeCell, bigArr);
    }
}

int main()
{
    mySimpleComputer sc;
    bigChars bc;
    readKey rk;
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
    if (bc.gotoXY(24, 84) == M) {
        cout << " Недостаточный размер терминала, минимум (24x84)\n";
        return -1;
    }
    bc.getscreensize(&termH, &termW);

    bordersUpdate(bc, stInput);

    ////////////////////////////////////////////// Динамические подписи
    regUpdate(bc, sc);

    int el; // проверка заполнением
    for (int i = 0; i < 100; i++) {
        el = 0;
        sc.commandEncode(10, i, &el);
        sc.memorySet(i, el);
    }

    memUpdate(bc, sc, activeCell, bigArr);

    clearInput(bc, stInput);
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
                bordersUpdate(bc, stInput);
                memUpdate(bc, sc, activeCell, bigArr);
                regUpdate(bc, sc);
            } while (strlen(fileName) == 4);
            if (sc.memoryLoad(fileName))
                clearInput(bc, "Файл " + string(fileName) + " не найден");
            // !!!!!!!!!!!!! добавить задержку
            memUpdate(bc, sc, activeCell, bigArr);
            rk.mytermSave();
            break;
        case s: // сохранение ram в файл
            rk.mytermRestore();
            do {
                clearInput(bc, fileNameInput);
                rk.readS(fileName);
                strcat(fileName, ".dat");
                bordersUpdate(bc, stInput);
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
            changeValue(bc, sc, rk, activeCell, bigArr);
            break;
        case q: // выход из программы
            break;
        }
        if (key != zeroKey) {
            regUpdate(bc, sc);
            clearInput(bc, stInput);
        }
    } while (key != q);
    bc.clrscr();
    rk.mytermRestore();
    return 0;
}
