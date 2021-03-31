#include "libmyscomp.hpp"

int mySimpleComputer::memoryInit()
{
    for (int i = 0; i < n; i++)
        arr[i] = 0;
    return 0;
}

int mySimpleComputer::memorySet(int address, int value)
{
    if (address >= 0 && address < n)
        arr[address] = value;
    else {
        regSet(M, 1);
        return M;
    }
    return 0;
}

int mySimpleComputer::memoryGet(int address, int* value)
{
    if (address >= 0 && address < n)
        *value = arr[address];
    else {
        regSet(M, 1);
        return M;
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
        regSet(E, 1);
        return E;
    }
    fread(&arr, sizeof(int), n, fp);
    fclose(fp);
    return 0;
}

int mySimpleComputer::regInit()
{
    regFlags = 0;
    counter = 0;
    accumulator = 0;
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
            regSet(M, 1);
            return M;
        }
    else {
        regSet(M, 1);
        return M;
    }
    return 0;
}

int mySimpleComputer::regGet(int reg, int* value)
{
    if (reg > 0 && reg <= int(sizeof(char) * 8))
        *value = getFlag(regFlags, reg);
    else {
        regSet(M, 1);
        return M;
    }
    return 0;
}

int mySimpleComputer::accumulatorSet(int value)
{
    if (value < 1 << 14)
        accumulator = value;
    else {
        regSet(M, 1);
        return M;
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
        counter = char(position);
    else {
        regSet(M, 1);
        return M;
    }
    return 0;
}
int mySimpleComputer::counterGet(int* position)
{
    *position = int(counter);
    return 0;
}

int mySimpleComputer::commandEncode(int command, int operand, int* value)
{
    if (rightCommand(command) && operand >= 0 && operand < (1 << 7)) {
        *value = 0;
        *value |= operand;
        command <<= 7;
        *value |= command;
    } else {
        regSet(E, 1);
        return E;
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
        regSet(E, 1);
        return E;
    }
    return 0;
}
