#include "bsc.hpp"

static var alfabet[26];
static memMap map[256];
static memMap jumpforward[256];
static int indexmap;
static int indexjump;
static int bgn;
static int endd;
static int tmp;
static int tmp1;
static int flagend;
static char readc[12] = "   READ   \n";
static char writec[13] = "   WRITE   \n";
static char jmpc[12] = "   JUMP   \n";
static char jneg[12] = "   JNEG   \n";
static char jns[11] = "   JNS   \n";
static char haltc[12] = "   HALT   \n";
static char load[12] = "   LOAD   \n";
static char store[13] = "   STORE   \n";
static char add[11] = "   ADD   \n";
static char mul[11] = "   MUL   \n";
static char divide[14] = "   DIVIDE   \n";
static char sub[11] = "   SUB   \n";
static char jz[10] = "   JZ   \n";

struct tOP* head;

int swriteInt(char* buff, int num, int radix, int znac)
{
    char sign = '-', buffer[256] = {0};
    int i = 0, j = 0, k = 0, counter = 256;
    if (num == 0) {
        if (znac == -1)
            buff[i] = '0';
        for (i = 0; i < znac; i++)
            buff[i] = '0';
        return i;
    }
    if (radix == 10) {
        if (num < 0) {
            buff[0] = sign;
            num = -num;
        }
    }
    while (num) {
        if (num % radix > 9) {
            buffer[--counter] = 'A' + num % radix - 10;
            num /= radix;
        } else {
            buffer[--counter] = '0' + (num % radix);
            num /= radix;
        }
    }
    if (znac > -1) {
        if ((256 - counter) < znac) {
            znac = znac - 256 + counter;
            while (znac) {
                buff[i] = '0';
                i++;
                j++;
                znac--;
            }
        }
    }
    while (i < 256 - counter + j) {
        buff[i] = buffer[counter + k++];
        i++;
    }
    return counter;
}
int prior(char c)
{
    switch (c) {
    case '(':
        return 1;
    case '+':
    case '-':
        return 2;
    case '*':
    case '/':
        return 3;
    default:
        return 0;
    }
}
void convert(char* str, char* out)
{
    int was_op = 0, np = 0;
    int j = 0;
    struct tOP* p;
    head = (tOP*)malloc(sizeof(struct tOP));
    head->next = NULL;
    for (int i = 0; i < strlen(str); i++) {
        if (((str[i] >= 'A') && (str[i] <= 'Z'))
            || ((str[i] >= '0') && (str[i] <= '9'))) {
            out[j] = str[i];
            j++;
            was_op = 0;
            continue;
        }
        switch (str[i]) {
        case '(': {
            p = (tOP*)malloc(sizeof(struct tOP));
            p->dt = str[i];
            p->next = head;
            head = p;
            np++;
            was_op = 0;
            break;
        }
        case '*':
        case '/':
        case '+':
        case '-': {
            if (!was_op) {
                was_op = 1;
                p = head;
                while (p != NULL) {
                    if (prior(str[i]) <= prior(p->dt)) {
                        out[j] = p->dt;
                        j++;
                        p = p->next;
                    } else
                        break;
                }
                head = p;
                p = (tOP*)calloc(1, sizeof(struct tOP));
                p->dt = str[i];
                p->next = head;
                head = p;
            }
            break;
        }
        case ')': {
            if (!was_op) {
                p = head;
                while (p->dt != '(' && np > 0) {
                    out[j] = p->dt;
                    j++;
                    p = p->next;
                }
                p = p->next;
            }
            head = p;
            np--;
            break;
        }
        case ' ':
            break;
        }
    }
    p = head;
    while (head != NULL) {
        out[j] = head->dt;
        j++;
        head = head->next;
        free(p);
        p = head;
    }
    out[j] = '\0';
}

int keywordCode(char* str)
{
    if (strcmp(str, "REM") == 0) {
        return KEYW_REM;
    } else if (strcmp(str, "INPUT") == 0) {
        return KEYW_INPUT;
    } else if (strcmp(str, "OUTPUT") == 0) {
        return KEYW_OUTPUT;
    } else if (strcmp(str, "GOTO") == 0) {
        return KEYW_GOTO;
    } else if (strcmp(str, "IF") == 0) {
        return KEYW_IF;
    } else if (strcmp(str, "LET") == 0) {
        return KEYW_LET;
    } else if ((strcmp(str, "END") == 0)) {
        return KEYW_END;
    } else {
        return KEYW_E;
    }
}

int basicTranslator(int argc, char* argv[])
{
    int input = 0, output = 0, readcount;
    char buffer[256] = {0};
    int i = 0;
    char* ptr = NULL;
    int ret = 0;
    tmp = -1;
    tmp1 = 0;
    bgn = 0;
    endd = 99;
    indexmap = 0;
    indexjump = 0;
    flagend = 0;

    if ((input = open(argv[1], O_RDONLY)) == -1)
        return 1;
    if ((output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
        return 1;
    for (i = 0; i < 26; i++) {
        alfabet[i].variable = 0;
        alfabet[i].name[0] = 'A' + i;
        alfabet[i].name[1] = '\0';
    }
    for (i = 0; i < 256; i++) {
        map[i].real = -1;
        map[i].expect = -1;
        jumpforward[i].real = -1;
        jumpforward[i].expect = -1;
    }
    i = 0;
    do {
        readcount = read(input, &buffer[i], sizeof(char));
        if (readcount == 0)
            break;
        if (buffer[i] == ' ') {
            if (buffer[i - 1] == ' ') {
                continue;
            }
        }
        if ((buffer[i] == '\n') || (buffer[i] == EOF)) {
            if (buffer[i] == EOF) {
                break;
            }
            buffer[i] = '\0';
            ret = parsingLineB(buffer, output);
            if (ret == 1)
                break;
            if (ret == 2)
                return 1;
            if (flagend) {
                for (int k = 0; k < indexjump; k++) {
                    jmpc[0] = map[jumpforward[k].expect].real / 10 + '0';
                    jmpc[1] = map[jumpforward[k].expect].real % 10 + '0';
                    int cell = jumpforward[k].real;
                    int kk;
                    for (kk = 0; kk < indexmap; kk++) {
                        if ((cell == map[kk].expect)) {
                            if (map[kk].real == -1) {
                                return 2;
                            }
                            cell = map[kk].real;
                            jmpc[8] = (cell) / 10 + '0';
                            jmpc[9] = (cell) % 10 + '0';
                            write(output, jmpc, sizeof(char) * strlen(jmpc));
                            break;
                        }
                    }
                    if (kk == indexmap) {
                        return 1;
                    }
                }
            }
            i = -1;
        }
        i++;
    } while (1);
    close(input);
    // printf("\n\n\n");
    // for (i = 0; i < indexmap; i++) {
    //     printf("%d       %d\n", map[i].real, map[i].expect);
    // }
    close(output);
    return 0;
}

int parsingLineB(char* str, int output)
{
    char tmpPtr1[256];
    char* ptr = str;
    char* tmpPtr = ptr;
    char* op = NULL;
    int command = 0, operand = 0, i = 0, j = 0, k = 0, q = 0, z = 0;
    int ret = 0, flagJump = 0, len, sk = 0, lastWrite = 0;
    while (1) {
        if (tmpPtr[i] == ' ') {
            ptr = tmpPtr + i + 1;
            tmpPtr[i] = '\0';
            break;
        } else if (tmpPtr[i] == EOF) {
            return 0;
        }
        i++;
    }
    if (!(str[0] >= '0' && str[0] <= '9')) {
        return 0;
    }
    if (tmp1 > 0)
        tmp = tmp1;
    sreadInt(tmpPtr, &tmp1, 10);
    if (tmp1 <= tmp) {
        return 2;
    }
    i = 0;
    tmpPtr = ptr;
    while (1) {
        if (tmpPtr[i] == ' ') {
            ptr = tmpPtr + i + 1;
            tmpPtr[i] = '\0';
            break;
        }
        i++;
    }
    ret = keywordCode(tmpPtr);
    tmpPtr = ptr;

    if (ret == KEYW_IF) {
        i = 0;
        while (1) {
            if (tmpPtr[i] == ' ') {
                ptr = tmpPtr + i + 1;
                tmpPtr[i] = '\0';
                break;
            }
            i++;
        }
        if (*tmpPtr >= 'A' & *tmpPtr <= 'Z') {
            for (i = 0; i < 26; i++) {
                if (!strcmp(alfabet[i].name, tmpPtr)) {
                    if (!(alfabet[i].variable == 1)) {
                        return 2;
                    }
                    tmpPtr = ptr;
                    j = 0;
                    while (1) {
                        if (tmpPtr[j] == ' ') {
                            ptr = tmpPtr + j + 1;
                            tmpPtr[j] = '\0';
                            break;
                        }
                        j++;
                    }
                    map[indexmap].real = bgn;
                    map[indexmap].expect = tmp1;
                    indexmap++;
                    op = tmpPtr;
                    tmpPtr = ptr;
                    load[0] = bgn / 10 + '0';
                    load[1] = bgn++ % 10 + '0';
                    load[8] = alfabet[i].cell / 10 + '0';
                    load[9] = alfabet[i].cell % 10 + '0';
                    write(output, load, sizeof(char) * strlen(load));
                    j = 0;
                    while (1) {
                        if (tmpPtr[j] == ' ') {
                            ptr = tmpPtr + j + 1;
                            tmpPtr[j] = '\0';
                            break;
                        }
                        j++;
                    }
                    if (*tmpPtr >= 'A' & *tmpPtr <= 'Z') {
                        for (j = 0; j < 26; j++) {
                            if (!strcmp(alfabet[j].name, tmpPtr)) {
                                tmpPtr = ptr;
                                if (!(alfabet[j].variable == 1)) {
                                    return 2;
                                }
                                sub[0] = bgn / 10 + '0';
                                sub[1] = bgn++ % 10 + '0';
                                sub[7] = alfabet[j].cell / 10 + '0';
                                sub[8] = alfabet[j].cell % 10 + '0';
                                write(output, sub, sizeof(char) * strlen(sub));
                                break;
                            }
                        }

                    } else {
                        int number = atoi(tmpPtr);
                        int addr = endd--;
                        char buf[4];
                        swriteInt(buf, number, 16, 4);
                        char s = (addr) / 10 + '0';
                        write(output, &s, 1);
                        s = (addr) % 10 + '0';
                        write(output, &s, 1);
                        write(output, " = ", 3);
                        write(output, buf, 4);
                        write(output, "\n", 1);
                        sub[0] = bgn / 10 + '0';
                        sub[1] = bgn++ % 10 + '0';
                        sub[7] = addr / 10 + '0';
                        sub[8] = addr % 10 + '0';
                        write(output, sub, sizeof(char) * strlen(sub));
                    }
                }
            }
        } else {
            int number = atoi(tmpPtr);
            int addr = endd--;
            char buf[4];
            swriteInt(buf, number, 16, 4);
            char s = (addr) / 10 + '0';
            write(output, &s, 1);
            s = (addr) % 10 + '0';
            write(output, &s, 1);
            write(output, " = ", 3);
            write(output, buf, 4);
            write(output, "\n", 1);
            load[0] = bgn / 10 + '0';
            load[1] = bgn++ % 10 + '0';
            load[8] = addr / 10 + '0';
            load[9] = addr % 10 + '0';
            write(output, load, sizeof(char) * strlen(load));
            tmpPtr = ptr;
            j = 0;
            while (1) {
                if (tmpPtr[j] == ' ') {
                    ptr = tmpPtr + j + 1;
                    tmpPtr[j] = '\0';
                    break;
                }
                j++;
            }
            op = tmpPtr;
            tmpPtr = ptr;
            j = 0;
            while (1) {
                if (tmpPtr[j] == ' ') {
                    ptr = tmpPtr + j + 1;
                    tmpPtr[j] = '\0';
                    break;
                }
                j++;
            }
            if (*tmpPtr >= 'A' & *tmpPtr <= 'Z') {
                for (j = 0; j < 26; j++) {
                    if (!strcmp(alfabet[j].name, tmpPtr)) {
                        tmpPtr = ptr;
                        if (!(alfabet[j].variable == 1)) {
                            return 2;
                        }
                        sub[0] = bgn / 10 + '0';
                        sub[1] = bgn++ % 10 + '0';
                        sub[7] = alfabet[j].cell / 10 + '0';
                        sub[8] = alfabet[j].cell % 10 + '0';
                        write(output, sub, sizeof(char) * strlen(sub));
                        break;
                    }
                }
            } else {
                number = atoi(tmpPtr);
                addr = endd--;
                swriteInt(buf, number, 16, 4);
                char s = (addr) / 10 + '0';
                write(output, &s, 1);
                s = (addr) % 10 + '0';
                write(output, &s, 1);
                write(output, " = ", 3);
                write(output, buf, 4);
                write(output, "\n", 1);
                sub[0] = bgn / 10 + '0';
                sub[1] = bgn++ % 10 + '0';
                sub[7] = addr / 10 + '0';
                sub[8] = addr % 10 + '0';
                write(output, sub, sizeof(char) * strlen(sub));
            }
        }
        tmpPtr = ptr;
        j = 0;
        while (1) {
            if (tmpPtr[j] == ' ') {
                ptr = tmpPtr + j + 1;
                tmpPtr[j] = '\0';
                break;
            }
            j++;
        }
        ret = keywordCode(tmpPtr);
        tmpPtr = ptr;
        flagJump = 1;
    }
    if (ret == KEYW_GOTO) {
        if (flagJump == 1) {
            flagJump = 0;
            int ret = ifoperation(output, op, 0, 0);
            if (ret == 1) {
                return 1;
            }
        }
        jmpc[0] = bgn / 10 + '0';
        jmpc[1] = bgn % 10 + '0';
        map[indexmap].real = bgn;
        map[indexmap].expect = tmp1;
        indexmap++;
        int cell = (tmpPtr[0] - '0') * 10 + (tmpPtr[1] - '0');
        for (i = 0; i < indexmap; i++) {
            if ((cell == map[i].expect)) {
                if (map[i].real == -1) {
                    return 2;
                }
                cell = map[i].real;
                jmpc[8] = (cell) / 10 + '0';
                jmpc[9] = (cell) % 10 + '0';
                write(output, jmpc, sizeof(char) * strlen(jmpc));
                break;
            }
        }
        if (i == indexmap) {
            // jmpc[8] = (cell) / 10 + '0';
            // jmpc[9] = (cell) % 10 + '0';
            jumpforward[indexjump].real = cell;
            jumpforward[indexjump].expect = indexmap - 1;
            indexjump++;
        }
        bgn++;
    }
    if (ret == KEYW_REM) {
        map[indexmap].real = -1;
        map[indexmap].expect = tmp1;
        indexmap++;
        return 0;
    } else if (ret == KEYW_INPUT) {
        if (flagJump == 1) {
            flagJump = 0;
            int ret = ifoperation(output, op, 0, 0);
            if (ret == 1) {
                return 1;
            }
        }
        for (i = 0; i < 26; i++) {
            if (!strcmp(alfabet[i].name, tmpPtr)) {
                if (endd - 1 < bgn) {
                    return 2;
                }
                alfabet[i].cell = endd--;
                if (alfabet[i].variable == 0) {
                    alfabet[i].variable = 1;
                }
                map[indexmap].real = bgn;
                map[indexmap].expect = tmp1;
                ++indexmap;
                readc[0] = bgn / 10 + '0';
                readc[1] = bgn++ % 10 + '0';
                readc[8] = alfabet[i].cell / 10 + '0';
                readc[9] = alfabet[i].cell % 10 + '0';
                write(output, readc, sizeof(char) * strlen(readc));
                break;
            }
        }
    } else if (ret == KEYW_OUTPUT) {
        if (flagJump == 1) {
            flagJump = 0;
            int ret = ifoperation(output, op, 0, 0);
            if (ret == 1) {
                return 1;
            }
        }
        for (i = 0; i < 26; i++) {
            if (!strcmp(alfabet[i].name, tmpPtr)) {
                tmpPtr = ptr;
                if (endd - 1 < bgn) {
                    return 2;
                }
                if (alfabet[i].variable == 0) {
                    return 2;
                }
                map[indexmap].real = bgn;
                map[indexmap].expect = tmp1;
                ++indexmap;
                writec[0] = bgn / 10 + '0';
                writec[1] = bgn++ % 10 + '0';
                writec[9] = alfabet[i].cell / 10 + '0';
                writec[10] = alfabet[i].cell % 10 + '0';
                write(output, writec, sizeof(char) * strlen(writec));
                break;
            }
        }
    } else if (ret == KEYW_LET) {
        len = strlen(ptr);
        for (i = 0; i < len; i++) {
            if (ptr[i] == '+' || ptr[i] == '-' || ptr[i] == '/'
                || ptr[i] == '*')
                sk++;
        }
        if (flagJump == 1) {
            flagJump = 0;
            int ret = ifoperation(output, op, 1, sk);
            if (ret == 1)
                return 1;
        }
        j = 0;
        while (1) {
            if (tmpPtr[j] == ' ') {
                ptr = tmpPtr + j + 1;
                tmpPtr[j] = '\0';
                break;
            }
            j++;
        }
        for (k = 0; k < 26; k++) {
            if (!strcmp(alfabet[k].name, tmpPtr)) {
                tmpPtr = ptr;
                if (alfabet[k].variable == 0) {
                    if (endd - 1 < bgn)
                        return 2;
                    alfabet[k].variable = 1;
                    alfabet[k].cell = endd--;
                }
                map[indexmap].real = bgn;
                map[indexmap].expect = tmp1;
                indexmap++;
                break;
            }
        }
        if (k == 26)
            return 2;
        j = 0;
        while (1) {
            if (tmpPtr[j] == ' ') {
                ptr = tmpPtr + j + 1;
                tmpPtr[j] = '\0';
                break;
            }
            j++;
        }
        if (!(strcmp(tmpPtr, "=") == 0))
            return 2;
        convert(ptr, tmpPtr1);
        tmpPtr = ptr;
        len = strlen(tmpPtr1);
        if (sk == 0) {
            if (tmpPtr1[0] >= '0' && tmpPtr1[0] <= '9') {
                int number = atoi(&tmpPtr1[0]);
                int addr = endd--;
                char buf[4];
                swriteInt(buf, number, 16, 4);
                char s = (addr) / 10 + '0';
                write(output, &s, 1);
                s = (addr) % 10 + '0';
                write(output, &s, 1);
                write(output, " = ", 3);
                write(output, buf, 4);
                write(output, "\n", 1);
                load[0] = bgn / 10 + '0';
                load[1] = bgn++ % 10 + '0';
                load[8] = addr / 10 + '0';
                load[9] = addr % 10 + '0';
                write(output, load, sizeof(char) * strlen(load));
                store[0] = bgn / 10 + '0';
                store[1] = bgn++ % 10 + '0';
                store[9] = alfabet[k].cell / 10 + '0';
                store[10] = alfabet[k].cell % 10 + '0';
                write(output, store, sizeof(char) * strlen(store));
            } else if (tmpPtr1[0] >= 'A' && tmpPtr1[0] <= 'Z') {
                for (i = 0; i < 26; i++) {
                    if (tmpPtr1[0] == alfabet[i].name[0])
                        break;
                }
                if (i == 26)
                    return 2;
                load[0] = bgn / 10 + '0';
                load[1] = bgn++ % 10 + '0';
                load[8] = alfabet[i].cell / 10 + '0';
                load[9] = alfabet[i].cell % 10 + '0';
                write(output, load, sizeof(char) * strlen(load));
                store[0] = bgn / 10 + '0';
                store[1] = bgn++ % 10 + '0';
                store[9] = alfabet[k].cell / 10 + '0';
                store[10] = alfabet[k].cell % 10 + '0';
                write(output, store, sizeof(char) * strlen(store));
            } else
                return 2;
            return 0;
        }
        while (sk != 0) {
            // printf("\n$ %s $\n\n", tmpPtr1);
            int kk = 0;
            for (i = 0; i < len; i++) {
                if (tmpPtr1[i] == '+' || tmpPtr1[i] == '-' || tmpPtr1[i] == '*'
                    || tmpPtr1[i] == '/')
                    break;
            }
            for (j = 0; j < 26; j++) {
                if (alfabet[j].name[0] == tmpPtr1[i - 2]) {
                    break;
                }
            }
            for (q = 0; q < 26; q++) {
                if (alfabet[q].name[0] == tmpPtr1[i - 1]) {
                    break;
                }
            }
            if (alfabet[j].variable == 1 && alfabet[q].variable == 1) {
                load[0] = bgn / 10 + '0';
                load[1] = bgn++ % 10 + '0';
                load[8] = alfabet[j].cell / 10 + '0';
                load[9] = alfabet[j].cell % 10 + '0';
                write(output, load, sizeof(char) * strlen(load));
                switch (tmpPtr1[i]) {
                case '+': {
                    add[0] = bgn / 10 + '0';
                    add[1] = bgn++ % 10 + '0';
                    add[7] = alfabet[q].cell / 10 + '0';
                    add[8] = alfabet[q].cell % 10 + '0';
                    write(output, add, sizeof(char) * strlen(add));
                    break;
                }
                case '-': {
                    sub[0] = bgn / 10 + '0';
                    sub[1] = bgn++ % 10 + '0';
                    sub[7] = alfabet[q].cell / 10 + '0';
                    sub[8] = alfabet[q].cell % 10 + '0';
                    write(output, sub, sizeof(char) * strlen(sub));
                    break;
                }
                case '*': {
                    mul[0] = bgn / 10 + '0';
                    mul[1] = bgn++ % 10 + '0';
                    mul[7] = alfabet[q].cell / 10 + '0';
                    mul[8] = alfabet[q].cell % 10 + '0';
                    write(output, mul, sizeof(char) * strlen(mul));
                    break;
                }
                case '/': {
                    divide[0] = bgn / 10 + '0';
                    divide[1] = bgn++ % 10 + '0';
                    divide[10] = alfabet[q].cell / 10 + '0';
                    divide[11] = alfabet[q].cell % 10 + '0';
                    write(output, divide, sizeof(char) * strlen(divide));
                    break;
                }
                }
                for (z = 25; z > 0; z--) {
                    if (alfabet[z].variable == 0) {
                        if (endd - 1 < bgn)
                            return 2;
                        alfabet[z].variable = 1;
                        alfabet[z].cell = endd--;
                        map[indexmap].real = bgn;
                        map[indexmap].expect = tmp1;
                        ++indexmap;
                        break;
                    }
                }
                store[0] = bgn / 10 + '0';
                store[1] = bgn++ % 10 + '0';
                store[9] = alfabet[z].cell / 10 + '0';
                store[10] = alfabet[z].cell % 10 + '0';
                lastWrite = z;
                write(output, store, sizeof(char) * strlen(store));
                tmpPtr1[i] = alfabet[z].name[0];
                tmpPtr1[i - 1] = '!';
                tmpPtr1[i - 2] = '!';
                len = strlen(tmpPtr1);
                for (j = 0; j < len; j++) {
                    if (tmpPtr1[j] == '!') {
                        len--;
                        for (kk = j; kk < len; kk++) {
                            tmpPtr1[kk] = tmpPtr1[kk + 1];
                        }
                        tmpPtr1[kk] = '\0';
                    }
                }
                sk--;
            } else if (
                    !(alfabet[j].variable == 1) && alfabet[q].variable == 1) {
                int number = atoi(&tmpPtr1[i - 2]);
                int addr = endd--;
                char buf[4];
                swriteInt(buf, number, 16, 4);
                char s = (addr) / 10 + '0';
                write(output, &s, 1);
                s = (addr) % 10 + '0';
                write(output, &s, 1);
                write(output, " = ", 3);
                write(output, buf, 4);
                write(output, "\n", 1);
                load[0] = bgn / 10 + '0';
                load[1] = bgn++ % 10 + '0';
                load[8] = addr / 10 + '0';
                load[9] = addr % 10 + '0';
                write(output, load, sizeof(char) * strlen(load));
                switch (tmpPtr1[i]) {
                case '+': {
                    add[0] = bgn / 10 + '0';
                    add[1] = bgn++ % 10 + '0';
                    add[7] = alfabet[q].cell / 10 + '0';
                    add[8] = alfabet[q].cell % 10 + '0';
                    write(output, add, sizeof(char) * strlen(add));
                    break;
                }
                case '-': {
                    sub[0] = bgn / 10 + '0';
                    sub[1] = bgn++ % 10 + '0';
                    sub[7] = alfabet[q].cell / 10 + '0';
                    sub[8] = alfabet[q].cell % 10 + '0';
                    write(output, sub, sizeof(char) * strlen(sub));
                    break;
                }
                case '*': {
                    mul[0] = bgn / 10 + '0';
                    mul[1] = bgn++ % 10 + '0';
                    mul[7] = alfabet[q].cell / 10 + '0';
                    mul[8] = alfabet[q].cell % 10 + '0';
                    write(output, mul, sizeof(char) * strlen(mul));
                    break;
                }
                case '/': {
                    divide[0] = bgn / 10 + '0';
                    divide[1] = bgn++ % 10 + '0';
                    divide[10] = alfabet[q].cell / 10 + '0';
                    divide[11] = alfabet[q].cell % 10 + '0';
                    write(output, divide, sizeof(char) * strlen(divide));
                    break;
                }
                }
                for (z = 25; z > 0; z--) {
                    if (alfabet[z].variable == 0) {
                        if (endd - 1 < bgn)
                            return 2;
                        alfabet[z].variable = 1;
                        alfabet[z].cell = endd--;
                        map[indexmap].real = bgn;
                        map[indexmap].expect = tmp1;
                        ++indexmap;
                        break;
                    }
                }
                store[0] = bgn / 10 + '0';
                store[1] = bgn++ % 10 + '0';
                store[9] = alfabet[z].cell / 10 + '0';
                store[10] = alfabet[z].cell % 10 + '0';
                lastWrite = z;
                write(output, store, sizeof(char) * strlen(store));
                tmpPtr1[i] = alfabet[z].name[0];
                tmpPtr1[i - 1] = '!';
                tmpPtr1[i - 2] = '!';
                len = strlen(tmpPtr1);
                for (j = 0; j < len; j++) {
                    if (tmpPtr1[j] == '!') {
                        len--;
                        for (kk = j; kk < len; kk++) {
                            tmpPtr1[kk] = tmpPtr1[kk + 1];
                        }
                        tmpPtr1[kk] = '\0';
                    }
                }
                sk--;
            } else if (
                    !(alfabet[q].variable == 1) && alfabet[j].variable == 1) {
                load[0] = bgn / 10 + '0';
                load[1] = bgn++ % 10 + '0';
                load[8] = alfabet[j].cell / 10 + '0';
                load[9] = alfabet[j].cell % 10 + '0';
                write(output, load, sizeof(char) * strlen(load));
                int number = atoi(&tmpPtr1[i - 1]);
                int addr = endd--;
                char buf[4];
                swriteInt(buf, number, 16, 4);
                char s = (addr) / 10 + '0';
                write(output, &s, 1);
                s = (addr) % 10 + '0';
                write(output, &s, 1);
                write(output, " = ", 3);
                write(output, buf, 4);
                write(output, "\n", 1);
                write(output, load, sizeof(char) * strlen(load));
                switch (tmpPtr1[i]) {
                case '+': {
                    add[0] = bgn / 10 + '0';
                    add[1] = bgn++ % 10 + '0';
                    add[7] = addr / 10 + '0';
                    add[8] = addr % 10 + '0';
                    write(output, add, sizeof(char) * strlen(add));
                    break;
                }
                case '-': {
                    sub[0] = bgn / 10 + '0';
                    sub[1] = bgn++ % 10 + '0';
                    sub[7] = addr / 10 + '0';
                    sub[8] = addr % 10 + '0';
                    write(output, sub, sizeof(char) * strlen(sub));
                    break;
                }
                case '*': {
                    mul[0] = bgn / 10 + '0';
                    mul[1] = bgn++ % 10 + '0';
                    mul[7] = addr / 10 + '0';
                    mul[8] = addr % 10 + '0';
                    write(output, mul, sizeof(char) * strlen(mul));
                    break;
                }
                case '/': {
                    divide[0] = bgn / 10 + '0';
                    divide[1] = bgn++ % 10 + '0';
                    divide[10] = addr / 10 + '0';
                    divide[11] = addr % 10 + '0';
                    write(output, divide, sizeof(char) * strlen(divide));
                    break;
                }
                }
                for (z = 25; z > 0; z--) {
                    if (alfabet[z].variable == 0) {
                        if (endd - 1 < bgn)
                            return 2;
                        alfabet[z].variable = 1;
                        alfabet[z].cell = endd--;
                        map[indexmap].real = bgn;
                        map[indexmap].expect = tmp1;
                        ++indexmap;
                        break;
                    }
                }
                store[0] = bgn / 10 + '0';
                store[1] = bgn++ % 10 + '0';
                store[9] = alfabet[z].cell / 10 + '0';
                store[10] = alfabet[z].cell % 10 + '0';
                lastWrite = z;
                write(output, store, sizeof(char) * strlen(store));
                tmpPtr1[i] = alfabet[z].name[0];
                tmpPtr1[i - 1] = '!';
                tmpPtr1[i - 2] = '!';
                len = strlen(tmpPtr1);
                for (j = 0; j < len; j++) {
                    if (tmpPtr1[j] == '!') {
                        len--;
                        for (kk = j; kk < len; kk++) {
                            tmpPtr1[kk] = tmpPtr1[kk + 1];
                        }
                        tmpPtr1[kk] = '\0';
                    }
                }
                sk--;
            } else {
                char l = tmpPtr1[i - 2];
                int number = atoi(&l);
                int addr = endd--;
                char buf[4];
                swriteInt(buf, number, 16, 4);
                char s = (addr) / 10 + '0';
                write(output, &s, 1);
                s = (addr) % 10 + '0';
                write(output, &s, 1);
                write(output, " = ", 3);
                write(output, buf, 4);
                write(output, "\n", 1);
                load[0] = bgn / 10 + '0';
                load[1] = bgn++ % 10 + '0';
                load[8] = addr / 10 + '0';
                load[9] = addr % 10 + '0';
                write(output, load, sizeof(char) * strlen(load));
                char ll = tmpPtr1[i - 1];
                number = atoi(&ll);
                addr = endd--;
                swriteInt(buf, number, 16, 4);
                s = (addr) / 10 + '0';
                write(output, &s, 1);
                s = (addr) % 10 + '0';
                write(output, &s, 1);
                write(output, " = ", 3);
                write(output, buf, 4);
                write(output, "\n", 1);
                switch (tmpPtr1[i]) {
                case '+': {
                    add[0] = bgn / 10 + '0';
                    add[1] = bgn++ % 10 + '0';
                    add[7] = addr / 10 + '0';
                    add[8] = addr % 10 + '0';
                    write(output, add, sizeof(char) * strlen(add));
                    break;
                }
                case '-': {
                    sub[0] = bgn / 10 + '0';
                    sub[1] = bgn++ % 10 + '0';
                    sub[7] = addr / 10 + '0';
                    sub[8] = addr % 10 + '0';
                    write(output, sub, sizeof(char) * strlen(sub));
                    break;
                }
                case '*': {
                    mul[0] = bgn / 10 + '0';
                    mul[1] = bgn++ % 10 + '0';
                    mul[7] = addr / 10 + '0';
                    mul[8] = addr % 10 + '0';
                    write(output, mul, sizeof(char) * strlen(mul));
                    break;
                }
                case '/': {
                    divide[0] = bgn / 10 + '0';
                    divide[1] = bgn++ % 10 + '0';
                    divide[10] = addr / 10 + '0';
                    divide[11] = addr % 10 + '0';
                    write(output, divide, sizeof(char) * strlen(divide));
                    break;
                }
                }
                for (z = 25; z > 0; z--) {
                    if (alfabet[z].variable == 0) {
                        if (endd - 1 < bgn)
                            return 2;
                        alfabet[z].variable = 1;
                        alfabet[z].cell = endd--;
                        map[indexmap].real = bgn;
                        map[indexmap].expect = tmp1;
                        ++indexmap;
                        break;
                    }
                }
                store[0] = bgn / 10 + '0';
                store[1] = bgn++ % 10 + '0';
                store[9] = alfabet[z].cell / 10 + '0';
                store[10] = alfabet[z].cell % 10 + '0';
                lastWrite = z;
                write(output, store, sizeof(char) * strlen(store));
                tmpPtr1[i] = alfabet[z].name[0];
                tmpPtr1[i - 1] = '!';
                tmpPtr1[i - 2] = '!';
                len = strlen(tmpPtr1);
                for (j = 0; j < len; j++) {
                    if (tmpPtr1[j] == '!') {
                        len--;
                        for (kk = j; kk < len; kk++) {
                            tmpPtr1[kk] = tmpPtr1[kk + 1];
                        }
                        tmpPtr1[kk] = '\0';
                    }
                }
                sk--;
            }
        }
        load[0] = bgn / 10 + '0';
        load[1] = bgn++ % 10 + '0';
        load[8] = alfabet[lastWrite].cell / 10 + '0';
        load[9] = alfabet[lastWrite].cell % 10 + '0';
        write(output, load, sizeof(char) * strlen(load));
        store[0] = bgn / 10 + '0';
        store[1] = bgn++ % 10 + '0';
        store[9] = alfabet[k].cell / 10 + '0';
        store[10] = alfabet[k].cell % 10 + '0';
        write(output, store, sizeof(char) * strlen(store));
    } else if (ret == KEYW_END) {
        flagend = 1;
        if (flagJump == 1) {
            flagJump = 0;
            int ret = ifoperation(output, op, 0, 0);
            if (ret == 1) {
                return 1;
            }
        }
        map[indexmap].real = bgn;
        map[indexmap].expect = tmp1;
        ++indexmap;
        haltc[0] = bgn / 10 + '0';
        haltc[1] = bgn++ % 10 + '0';
        haltc[8] = '0';
        haltc[9] = '0';
        write(output, haltc, sizeof(char) * strlen(haltc));
        return 0;
    } else if (ret == KEYW_E) {
        return 0;
    }
    return 0;
}

int ifoperation(int output, char* op, int let, int zn)
{
    if (let == 0) {
        if (!strcmp(op, ">")) {
            jz[0] = bgn / 10 + '0';
            jz[1] = bgn % 10 + '0';
            jz[6] = (bgn + 3) / 10 + '0';
            jz[7] = (bgn++ + 3) % 10 + '0';
            write(output, jz, sizeof(char) * strlen(jz));
            jneg[0] = bgn / 10 + '0';
            jneg[1] = bgn % 10 + '0';
            jneg[8] = (bgn + 2) / 10 + '0';
            jneg[9] = (bgn++ + 2) % 10 + '0';
            write(output, jneg, sizeof(char) * strlen(jneg));
            return 0;
        } else if (!strcmp(op, "<")) {
            jns[0] = bgn / 10 + '0';
            jns[1] = bgn % 10 + '0';
            jns[7] = (bgn + 3) / 10 + '0';
            jns[8] = (bgn++ + 3) % 10 + '0';
            write(output, jns, sizeof(char) * strlen(jns));
            jz[0] = bgn / 10 + '0';
            jz[1] = bgn % 10 + '0';
            jz[6] = (bgn + 2) / 10 + '0';
            jz[7] = (bgn++ + 2) % 10 + '0';
            write(output, jz, sizeof(char) * strlen(jz));
            return 0;
        } else if (!strcmp(op, "=")) {
            jneg[0] = bgn / 10 + '0';
            jneg[1] = bgn % 10 + '0';
            jneg[8] = (bgn + 3) / 10 + '0';
            jneg[9] = (bgn++ + 3) % 10 + '0';
            write(output, jneg, sizeof(char) * strlen(jneg));
            jns[0] = bgn / 10 + '0';
            jns[1] = bgn % 10 + '0';
            jns[7] = (bgn + 2) / 10 + '0';
            jns[8] = (bgn++ + 2) % 10 + '0';
            write(output, jns, sizeof(char) * strlen(jns));
            return 0;
        } else {
            return 1;
        }
    } else if (let == 1) {
        if (!strcmp(op, ">")) {
            jz[0] = bgn / 10 + '0';
            jz[1] = bgn % 10 + '0';
            jz[6] = (bgn + (3 * zn + 4)) / 10 + '0';
            jz[7] = (bgn++ + (3 * zn + 4)) % 10 + '0';
            write(output, jz, sizeof(char) * strlen(jz));
            jneg[0] = bgn / 10 + '0';
            jneg[1] = bgn % 10 + '0';
            jneg[8] = (bgn + (3 * zn + 3)) / 10 + '0';
            jneg[9] = (bgn++ + (3 * zn + 3)) % 10 + '0';
            write(output, jneg, sizeof(char) * strlen(jneg));
            return 0;
        } else if (!strcmp(op, "<")) {
            jns[0] = bgn / 10 + '0';
            jns[1] = bgn % 10 + '0';
            jns[7] = (bgn + (3 * zn + 4)) / 10 + '0';
            jns[8] = (bgn++ + (3 * zn + 4)) % 10 + '0';
            write(output, jns, sizeof(char) * strlen(jns));
            jz[0] = bgn / 10 + '0';
            jz[1] = bgn % 10 + '0';
            jz[6] = (bgn + (3 * zn + 3)) / 10 + '0';
            jz[7] = (bgn++ + (3 * zn + 3)) % 10 + '0';
            write(output, jz, sizeof(char) * strlen(jz));
            return 0;
        } else if (!strcmp(op, "=")) {
            jneg[0] = bgn / 10 + '0';
            jneg[1] = bgn % 10 + '0';
            jneg[8] = (bgn + (3 * zn + 4)) / 10 + '0';
            jneg[9] = (bgn++ + (3 * zn + 4)) % 10 + '0';
            write(output, jneg, sizeof(char) * strlen(jneg));
            jns[0] = bgn / 10 + '0';
            jns[1] = bgn % 10 + '0';
            jns[7] = (bgn + (3 * zn + 3)) / 10 + '0';
            jns[8] = (bgn++ + (3 * zn + 3)) % 10 + '0';
            write(output, jns, sizeof(char) * strlen(jns));
            return 0;
        } else {
            return 1;
        }
    }
}
