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

#ifndef MY_INTERFACE
#define MY_INTERFACE

#define POS_ZERO 0x4000
#define NEG_ZERO 0x6000

#define POS_MAX 0x5FFF
#define NEG_MAX 0x7FFF

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
    bc.writeT(" Accumulator ");
    bc.gotoXY(4, 63);
    bc.writeT(" InstructionCounter ");
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
    regStr = "P";
    sc.regGet(P, &checkReg);
    if (checkReg)
        bc.setfgcolor(cl_red);
    bc.writeT(regStr);
    bc.setfgcolor();

    bc.gotoXY(11, 71);
    regStr = "O";
    sc.regGet(O, &checkReg);
    if (checkReg)
        bc.setfgcolor(cl_red);
    bc.writeT(regStr);
    bc.setfgcolor();

    bc.gotoXY(11, 73);
    regStr = "M";
    sc.regGet(M, &checkReg);
    if (checkReg)
        bc.setfgcolor(cl_red);
    bc.writeT(regStr);
    bc.setfgcolor();

    bc.gotoXY(11, 75);
    regStr = "T";
    sc.regGet(T, &checkReg);
    if (checkReg)
        bc.setfgcolor(cl_red);
    bc.writeT(regStr);
    bc.setfgcolor();

    bc.gotoXY(11, 77);
    regStr = "E";
    sc.regGet(E, &checkReg);
    if (checkReg)
        bc.setfgcolor(cl_red);
    bc.writeT(regStr);
    bc.setfgcolor();
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
    bc.gotoXY(24, 84);
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
    bc.gotoXY(24, 84);
}

void counterUpdate(bigChars& bc, mySimpleComputer& sc, int activeCell)
{
    string value;
    ostringstream s;
    int counter;
    sc.counterGet(&counter);

    if (activeCell == 101) {
        bc.setbgcolor(cl_gray);
        bc.setfgcolor(cl_black);
    }
    s.str("");
    s << setfill('0') << setw(4) << hex << counter;
    value = s.str();
    bc.gotoXY(5, 71);
    bc.writeT(value);
    bc.setbgcolor();
    bc.setfgcolor();
}

void accumulatorUpdate(bigChars& bc, mySimpleComputer& sc, int activeCell)
{
    string value;
    ostringstream s;
    int accum;
    sc.accumulatorGet(&accum);

    if (activeCell == 100) {
        bc.setbgcolor(cl_gray);
        bc.setfgcolor(cl_black);
    }
    s << setfill('0') << setw(4) << hex << (accum & 0x1FFF);
    if (getFlag(accum, 13))
        value = "-";
    else
        value = " ";
    value += s.str();
    bc.gotoXY(2, 70);
    bc.writeT(value);
    bc.setbgcolor();
    bc.setfgcolor();
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

    accumulatorUpdate(bc, sc, activeCell);
    counterUpdate(bc, sc, activeCell);
}

void changeValue(
        bigChars& bc, mySimpleComputer& sc, int activeCell, int* bigArr)
{
    if (activeCell == 100) {
        int el;
        sc.accumulatorGet(&el);
        int activeBigChar = 0;
        bigCharUpdate(el, bc, bigArr, activeBigChar);
        keys key;
        do {
            key = zeroKey;
            sc.readK(&key);
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
                    if (getFlag(el, 13)) {
                        wr0(el, 13);
                    } else {
                        wr1(el, 13);
                    }
                } else if (activeBigChar == 1) {
                    if ((el | 0xFF) == NEG_MAX)
                        el = (el & 0xFF) + NEG_ZERO;
                    else if ((el | 0xFF) == POS_MAX)
                        el = (el & 0xFF) + POS_ZERO;
                    else
                        el += (1 << 8);
                } else if (activeBigChar == 2) {
                    if (el == NEG_MAX)
                        el = NEG_ZERO;
                    else if (el == POS_MAX)
                        el = POS_ZERO;
                    else
                        el++;
                }
            } else if (key == downKey) {
                if (activeBigChar == 0) {
                    if (getFlag(el, 13)) {
                        wr0(el, 13);
                    } else {
                        wr1(el, 13);
                    }
                } else if (activeBigChar == 1) {
                    if (el >> 8 == NEG_ZERO >> 8)
                        el = (el & 0xFF) + ((NEG_MAX >> 8) << 8);
                    else if (el >> 8 == POS_ZERO >> 8)
                        el = (el & 0xFF) + ((POS_MAX >> 8) << 8);
                    else
                        el -= (1 << 8);
                } else if (activeBigChar == 2) {
                    if (el == NEG_ZERO)
                        el = NEG_MAX;
                    else if (el == POS_ZERO)
                        el = POS_MAX;
                    else
                        el--;
                }
            }
            if (key == enter) {
                sc.accumulatorSet(el);
                memUpdate(bc, sc, activeCell, bigArr);
            }
            if (key != zeroKey) {
                bigCharUpdate(el, bc, bigArr, activeBigChar);
            }
        } while (key != enter);
    } else if (activeCell == 101) {
        int el;
        sc.counterGet(&el);
        keys key;
        do {
            key = zeroKey;
            sc.readK(&key);
            if (key == upKey) {
                el++;
                if (el > 99)
                    el = 0;
            } else if (key == downKey) {
                el--;
                if (el < 0)
                    el = 99;
            }
            sc.counterSet(el);
            if (key != zeroKey)
                memUpdate(bc, sc, activeCell, bigArr);
            bc.gotoXY(24, 84);
        } while (key != enter);
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
            sc.readK(&key);
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
                    if (isData) {
                        if (getFlag(el, 13)) {
                            wr0(el, 13);
                        } else {
                            isData = 0;
                            wr0(el, 14);
                            sc.commandDecode(el, &command, &operand);
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
                        if ((el | 0xFF) == NEG_MAX)
                            el = (el & 0xFF) + NEG_ZERO;
                        else if ((el | 0xFF) == POS_MAX)
                            el = (el & 0xFF) + POS_ZERO;
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
                    if (isData) {
                        if (getFlag(el, 13)) {
                            isData = 0;
                            wr0(el, 14);
                            sc.commandDecode(el, &command, &operand);
                        } else {
                            wr1(el, 13);
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
                        if (el >> 8 == NEG_ZERO >> 8)
                            el = (el & 0xFF) + ((NEG_MAX >> 8) << 8);
                        else if (el >> 8 == POS_ZERO >> 8)
                            el = (el & 0xFF) + ((POS_MAX >> 8) << 8);
                        else
                            el -= (1 << 8);
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
                memUpdate(bc, sc, activeCell, bigArr);
            }
            if (key != zeroKey) {
                if (!isData) {
                    bigCharUpdate(command, operand, bc, bigArr, activeBigChar);
                } else
                    bigCharUpdate(el, bc, bigArr, activeBigChar);
            }
        } while (key != enter);
        memUpdate(bc, sc, activeCell, bigArr);
    }
}

#endif
