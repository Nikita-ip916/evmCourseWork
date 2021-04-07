#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

#ifndef MY_SIMPLE_COMPUTER
#define MY_SIMPLE_COMPUTER

#define P 1 // переполнение при выполнении операции
#define O 2 // ошибка деления на нуль
#define M 3 // выход за границы памяти
#define T 4 // игнорирование тактовых импульсов
#define E 5 // указана неверная команда

#define rightCommand(command)                                                \
    ((command >= 10 && command <= 11) || (command >= 20 && command <= 21)    \
     || (command >= 30 && command <= 33) || (command >= 40 && command <= 43) \
     || (command == 76))

#define wr1(regFlags, reg) regFlags |= (1 << reg)
#define wr0(regFlags, reg) regFlags &= ~(1 << reg)
#define getFlag(regFlags, reg) (regFlags >> reg) & 1

const int n = 100;

class mySimpleComputer {
    int arr[n];
    char regFlags;
    char counter;
    short int accumulator;

public:
    int memoryInit();

    int memorySet(int address, int value);

    int memoryGet(int address, int* value);

    int memorySave(char* filename);

    int memoryLoad(char* filename);

    int regInit();

    int regSet(int reg, int value);

    int regGet(int reg, int* value);

    int accumulatorSet(int value);
    int accumulatorGet(int* value);

    int counterSet(int position);
    int counterGet(int* position);

    int commandEncode(int command, int operand, int* value);

    int commandDecode(int value, int* command, int* operand);
};

#endif
