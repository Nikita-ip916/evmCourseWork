#include "asm.hpp"

int myPow(int a, int b)
{
    int tmp = a;
    if (b == 0)
        return 1;
    else if (b == 1)
        return a;
    for (int i = 1; i < b; i++)
        a *= tmp;
    return a;
}

int sreadInt(char* buffers, int* num, int radix)
{
    int size = strlen(buffers) - 1;
    int i = size, k = 0;
    *num = 0;
    for (i = size; i >= 0; i--)
        if ((buffers[i] >= '0') & (buffers[i] <= '9'))
            *num += myPow(radix, k++) * (buffers[i] - '0');
        else if ((buffers[i] >= 'A') & (buffers[i] <= 'F'))
            *num += myPow(radix, k++) * (buffers[i] - 'A' + 10);
        else
            return -1;
    return 1;
}

int sc_commandEncode(int command, int operand, int* value)
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

int asmTranslator(int argc, char** argv)
{
    if (checkArgv(argv) == 1)
        return 1;
    int inputAsm = 0, outputBin = 0, counterTokens = 0, flagign = 0, i = 0;
    int address = 0, value = 0, ret = 0;
    char buffer[512] = {0};
    int ram[n];
    if ((inputAsm = open(argv[1], O_RDONLY)) == -1)
        return 1;
    if ((outputBin = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
        return 1;
    for (i = 0; i < n; i++)
        ram[i] = 0;
    i = 0;
    do {
        value = 0;
        address = 0;
        int readcount = read(inputAsm, &buffer[i], sizeof(char));
        if (readcount == 0)
            break;
        if (buffer[i] == ' ') {
            if (buffer[i - 1] == ' ') {
                continue;
            }
        }
        if (buffer[i] == EOF && flagign == 0) {
            if (counterTokens == 2) {
                buffer[i] = '\0';
                ret = parsingLine(buffer, &address, &value);
                if (!ret) {
                    ram[address] = value;
                }
            }
            break;
        }
        if (buffer[i] == '\n' && flagign == 1) {
            flagign = 0;
            i = 0;
            counterTokens = 0;
            continue;
        }
        if (flagign == 0) {
            if (buffer[i] == ' ' && counterTokens < 2) {
                counterTokens++;
                if (counterTokens == 2) {
                    i++;
                }
            }
            if (counterTokens == 2
                & (buffer[i] == ';' || buffer[i] == '\n' || buffer[i] == ' ')) {
                if (buffer[i] == ';' || buffer[i] == ' ') {
                    flagign = 1;
                }
                buffer[i] = '\0';
                ret = parsingLine(buffer, &address, &value);
                if (ret == 0 || ret == 2) {
                    ram[address] = value;
                }
                counterTokens = 0;
                i = 0;
                continue;
            }
            i++;
        }
    } while (1);
    write(outputBin, ram, sizeof(int) * n);
    close(inputAsm);
    close(outputBin);
    return 0;
}

int checkArgv(char* argv[])
{
    char* ptr1 = NULL;
    char* ptr2 = NULL;
    ptr1 = strrchr(argv[1], '.');
    ptr2 = strrchr(argv[2], '.');
    if ((!(strcmp(ptr1, ".sa") == 0)) || (!(strcmp(ptr2, ".o") == 0))) {
        return 1;
    } else {
        return 0;
    }
}

int asmCommand(char* str)
{
    int ret = 0;
    if (strcmp(str, "READ") == 0) {
        ret = 10;
    } else if (strcmp(str, "WRITE") == 0) {
        ret = 11;
    } else if (strcmp(str, "LOAD") == 0) {
        ret = 20;
    } else if (strcmp(str, "STORE") == 0) {
        ret = 21;
    } else if (strcmp(str, "ADD") == 0) {
        ret = 30;
    } else if (strcmp(str, "SUB") == 0) {
        ret = 31;
    } else if (strcmp(str, "DIVIDE") == 0) {
        ret = 32;
    } else if (strcmp(str, "MUL") == 0) {
        ret = 33;
    } else if (strcmp(str, "JUMP") == 0) {
        ret = 40;
    } else if (strcmp(str, "JNEG") == 0) {
        ret = 41;
    } else if (strcmp(str, "JZ") == 0) {
        ret = 42;
    } else if (strcmp(str, "HALT") == 0) {
        ret = 43;
    } else if (strcmp(str, "SUBCT") == 0) {
        ret = 76;
    } else {
        ret = -1;
    }
    return ret;
}

int strToCommand(char* ptr, int* value)
{
    int command = 0, operand = 0, plusFlag = 0, position = 0, tmp = 0;
    if (*ptr == '+') {
        position++;
        plusFlag = 1;
    }
    if (sreadInt(ptr + position, &tmp, 16) != 1)
        return -1;
    operand = tmp & 0x7F;
    command = (tmp >> 8) & 0x7F;
    sc_commandEncode(command, operand, value);
    if (!plusFlag) {
        *value |= 1 << 14;
    }
    return 0;
}

int parsingLine(char* str, int* address, int* value)
{
    char* ptr = str;
    char* tmpPtr = ptr;
    int command = 0, operand = 0, flagCommand = 0, i = 0;
    while (1) {
        if (tmpPtr[i] == ' ') {
            ptr = tmpPtr + i + 1;
            tmpPtr[i] = '\0';
            break;
        }
        i++;
    }
    sreadInt(tmpPtr, address, 10);
    if (*address < 0 || *address > n)
        return -1;
    i = 0;
    tmpPtr = ptr;
    if (*ptr == '=')
        flagCommand = 1;
    if (flagCommand) {
        while (1) {
            if (tmpPtr[i] == ' ') {
                ptr = tmpPtr + i + 1;
                tmpPtr[i] = '\0';
                break;
            }
            i++;
        }
        tmpPtr = ptr + 1;
        if (*tmpPtr == '+') {
            char tmp = tmpPtr[3];
            tmpPtr[3] = '\0';
            sreadInt(tmpPtr + 1, &command, 16);
            tmpPtr[3] = tmp;
            tmpPtr = &tmpPtr[3];
            tmp = tmpPtr[2];
            tmpPtr[2] = '\0';
            sreadInt(tmpPtr, &operand, 16);
            tmpPtr[2] = tmp;
            sc_commandEncode(command, operand, value);
            //*value &= 0x7FFF;
        } else {
            sreadInt(tmpPtr, value, 16);
            *value |= (1 << 14);
            *value &= 0x7FFF;
        }
    } else {
        while (1) {
            if (tmpPtr[i] == ' ') {
                ptr = tmpPtr + i + 1;
                tmpPtr[i] = '\0';
                break;
            }
            i++;
        }
        command = asmCommand(tmpPtr);
        if (command == -1)
            return -1;
        i = 0;
        tmpPtr = ptr + 1;
        if (sreadInt(tmpPtr, &operand, 10) == -1)
            return -1;
        sc_commandEncode(command, operand, value);
    }
    if (command == 0x43)
        return 2;
    return 0;
}
