#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <libmyreadkey.hpp>
#include <signal.h>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

#ifndef MY_SIMPLE_COMPUTER
#define MY_SIMPLE_COMPUTER

#define P 1 // переполнение при выполнении операции
#define O 2 // ошибка деления на нуль
#define M 3 // выход за границы памяти
#define T 4 // игнорирование тактовых импульсов
#define E 5 // указана неверная команда

#define ALUcommand(command) \
    ((command >= 30 && command <= 33) || (command == 76))

#define CUcommand(command)                                                \
    ((command >= 10 && command <= 11) || (command >= 20 && command <= 21) \
     || (command >= 40 && command <= 43))

const int n = 100;

class mySimpleComputer : public readKey {
    static int memory[n];
    static char regFlags;
    static char instructionCounter;
    static short int accumulator;

public:
    static int memoryInit();

    static int memorySet(int address, int value);
    static int memoryGet(int address, int* value);

    static int memorySave(char* filename);
    static int memoryLoad(char* filename);

    static int regInit();

    static int regSet(int reg, int value);
    static int regGet(int reg, int* value);

    static int accumulatorSet(int value);
    static int accumulatorGet(int* value);

    static int counterSet(int position);
    static int counterGet(int* position);

    static int commandEncode(int command, int operand, int* value);
    static int commandDecode(int value, int* command, int* operand);

    static void signalHandler(int sigNum);

    static void clrInput();

    static int ALU(int command, int operand);
    static int CU();
};

#endif
