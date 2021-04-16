#include "libmyscomp.hpp"

int mySimpleComputer::arr[n];
char mySimpleComputer::regFlags;
char mySimpleComputer::instructionCounter;
short int mySimpleComputer::accumulator;

int mySimpleComputer::memoryInit()
{
    for (int i = 0; i < n; i++)
        arr[i] = 0;
    accumulator = 0x4000;
    return 0;
}

int mySimpleComputer::memorySet(int address, int value)
{
    if (address >= 0 && address < n)
        arr[address] = value;
    else {
        return -1;
    }
    return 0;
}

int mySimpleComputer::memoryGet(int address, int* value)
{
    if (address >= 0 && address < n)
        *value = arr[address];
    else {
        return -1;
    }
    return 0;
}

int mySimpleComputer::memorySave(char* filename)
{
    FILE* fp;
    fp = fopen(filename, "wb");
    fwrite(&arr, sizeof(int), n, fp);
    fclose(fp);
    return 0;
}

int mySimpleComputer::memoryLoad(char* filename)
{
    FILE* fp;
    fp = fopen(filename, "rb");
    if (!fp) {
        return -1;
    }
    fread(&arr, sizeof(int), n, fp);
    fclose(fp);
    return 0;
}

int mySimpleComputer::regInit()
{
    regFlags = 0;
    instructionCounter = 0;
    regSet(T, 1);
    return 0;
}

int mySimpleComputer::regSet(int reg, int value)
{
    if (reg > 0 && reg <= int(sizeof(char) * 8))
        if (value == 0)
            wr0(regFlags, reg);
        else if (value == 1)
            wr1(regFlags, reg);
        else {
            return -1;
        }
    else {
        return -1;
    }
    return 0;
}

int mySimpleComputer::regGet(int reg, int* value)
{
    if (reg > 0 && reg <= int(sizeof(char) * 8))
        *value = getFlag(regFlags, reg);
    else {
        return -1;
    }
    return 0;
}

int mySimpleComputer::accumulatorSet(int value)
{
    if (value < 1 << 15)
        accumulator = value;
    else {
        return -1;
    }
    return 0;
}
int mySimpleComputer::accumulatorGet(int* value)
{
    *value = accumulator;
    return 0;
}

int mySimpleComputer::counterSet(int position)
{
    if (position < 1 << 7)
        instructionCounter = char(position);
    else {
        return -1;
    }
    return 0;
}
int mySimpleComputer::counterGet(int* position)
{
    *position = int(instructionCounter);
    return 0;
}

int mySimpleComputer::commandEncode(int command, int operand, int* value)
{
    if (command >= 0 && command < (1 << 7) && operand >= 0
        && operand < (1 << 7)) {
        *value = 0;
        *value |= operand;
        command <<= 7;
        *value |= command;
    } else {
        return -1;
    }
    return 0;
}

int mySimpleComputer::commandDecode(int value, int* command, int* operand)
{
    if (value < (1 << 14)) {
        *command = 0;
        *operand = 0;
        *command |= (value >> 7);
        value &= ((1 << 7) - 1);
        *operand |= value;
    } else {
        return -1;
    }
    return 0;
}

void mySimpleComputer::signalHandler(int sigNum)
{
    switch (sigNum) {
    case SIGUSR1:
        alarm(0);
        regInit();
        regSet(T, 1);
        break;
    case SIGALRM:
        int value, instructionCounter;
        regGet(T, &value);
        if (!value) {
            counterGet(&instructionCounter);
            instructionCounter++;
            if (instructionCounter > 99)
                instructionCounter = 0;
            counterSet(instructionCounter);
            alarm(1);
        }
        break;
    }
}

int mySimpleComputer::ALU(int command, int operand)
{
    if (command == 30) {
    } else if (command == 31) {
    } else if (command == 32) {
    } else if (command == 33) {
    } else if (command == 76) {
    }
    return 0;
}

int mySimpleComputer::CU()
{
    int command, operand;
    if (operand < 0 || operand > 99)
        return -1;
    commandDecode(arr[instructionCounter], &command, &operand);
    if (ALUcommand(command)) {
        if (ALU(command, operand))
            return -1;
    } else if (command == 10) {
    } else if (command == 11) {
    } else if (command == 20) {
    } else if (command == 21) {
    } else if (command == 40) {
    } else if (command == 41) {
    } else if (command == 42) {
    } else if (command == 43) {
    } else {
        return -1;
    }
    return 0;
}
