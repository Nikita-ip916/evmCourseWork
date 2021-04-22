#include "libmyscomp.hpp"

int mySimpleComputer::memory[n];
char mySimpleComputer::regFlags;
char mySimpleComputer::instructionCounter;
short int mySimpleComputer::accumulator;
int mySimpleComputer::posInput;
int mySimpleComputer::posOutput;

int mySimpleComputer::memoryInit()
{
    for (int i = 0; i < n; i++)
        memory[i] = 0;
    accumulator = 0x4000;
    return 0;
}

int mySimpleComputer::memorySet(int address, int value)
{
    if (address >= 0 && address < n)
        memory[address] = value;
    else {
        return -1;
    }
    return 0;
}

int mySimpleComputer::memoryGet(int address, int* value)
{
    if (address >= 0 && address < n)
        *value = memory[address];
    else {
        return -1;
    }
    return 0;
}

int mySimpleComputer::memorySave(char* filename)
{
    FILE* fp;
    fp = fopen(filename, "wb");
    fwrite(&memory, sizeof(int), n, fp);
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
    fread(&memory, sizeof(int), n, fp);
    fclose(fp);
    return 0;
}

int mySimpleComputer::regInit()
{
    regFlags = 0;
    instructionCounter = 0;
    posInput = 0;
    posOutput = 0;
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
            if (CU())
                raise(SIGUSR1);
            else
                alarm(1);
        }
        break;
    }
}

int mySimpleComputer::ALU(int command, int operand)
{
    if (command == 30) { // ADD **
        if (!(((accumulator << 13) & 1) ^ ((memory[operand] << 13) & 1))) {
            if ((accumulator & ((1 << 13) - 1))
                        + (memory[operand] & ((1 << 13) - 1))
                < (1 << 13)) {
                accumulator += (memory[operand] & ((1 << 13) - 1));
            } else {
                regSet(P, 1);
            }
        } else {
            if ((accumulator & ((1 << 13) - 1))
                >= (memory[operand] & ((1 << 13) - 1))) {
                accumulator -= (memory[operand] & ((1 << 13) - 1));
            } else {
                accumulator = memory[operand] - (accumulator & ((1 << 13) - 1));
            }
        }
    } else if (command == 31) { // SUB ***
    } else if (command == 32) { // DIVIDE ***
    } else if (command == 33) { // MUL ***
    } else if (command == 76) { // SUBC ***
    }
    return 0;
}

void mySimpleComputer::clearString(string str, bool isInput)
{
    int tmp = str.length() + 1;
    for (int i = str.length() + 1; i <= 84; i++)
        str += " ";
    if (isInput)
        gotoXY(24, 1);
    else
        gotoXY(25, 1);
    writeT(str);
    if (isInput) {
        posInput = tmp;
        gotoXY(24, tmp - 15);
    } else {
        posOutput = tmp;
        gotoXY(25, tmp - 15);
    }
}

int mySimpleComputer::CU()
{
    int command, operand;
    if (commandDecode(memory[instructionCounter], &command, &operand)) {
        regSet(E, 1);
        return -1;
    } else if (operand < 0 || operand > 99) {
        regSet(E, 1);
        return -1;
    } else if (ALUcommand(command)) {
        if (ALU(command, operand))
            return -1;
    } else if (command == 10) { // READ **
        mytermRestore();
        setCursVis();
        char buffer[256] = "";
        gotoXY(24, posInput);
        readS(buffer);
        int val = 0;
        stringstream stream;
        stream << buffer;
        stream >> hex >> val;
        if (stream.fail()) {
            regSet(P, 1);
            setCursInv();
            mytermSave();
            return -1;
        } else if (val >= 0) {
            memory[operand] = (1 << 14) + (val & ((1 << 13) - 1));
            posInput += strlen(buffer) + 1;
        } else if (val < 0) {
            memory[operand] = (1 << 14) + (1 << 13) + (val & ((1 << 13) - 1));
            posInput += strlen(buffer) + 1;
        }
        setCursInv();
        mytermSave();
    } else if (command == 11) { // WRITE ***
    } else if (command == 20) { // LOAD *
    } else if (command == 21) { // STORE *
    } else if (command == 40) { // JUMP *
    } else if (command == 41) { // JNEG *
    } else if (command == 42) { // JZ *
    } else if (command == 43) { // HALT *
    } else {                    // Invalid command
        regSet(E, 1);
        return -1;
    }
    return 0;
}
