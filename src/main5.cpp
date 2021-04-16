#include "lab7.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;

struct Record {
    uint8_t active;
    string type;
    tCHS chs_start;
    tLBA l_start;
    tCHS chs_end;
    int32_t sectors;
    tLBA end;
};

struct Table {
    uint8_t full;
    struct Record records[4];
};

int main()
{
    tIDECHS idechs;
    int heads = 0;
    int sectors = 0;
    printf("Введите геометрию диска в формате IDECHS: \n");
    scanf("%hu%d%d", &idechs.cylinder, &heads, &sectors);
    idechs.head = (uint8_t)(heads);
    idechs.sector = (uint8_t)(sectors);
    double size = (((double)idechs.cylinder) * ((double)idechs.head)
                   * ((double)idechs.sector) * 512)
            / (1024 * 1024 * 1024);
    printf("Размер жёсткого диска: %lf GB\n", size);

    uint8_t was_active = 0;
    tLBA now_at = 1;
    uint32_t table_num = 0;
    struct Table tables[1024];
    double free_size = size;
    for (uint64_t i = 0; i < 1024; ++i) {
        tables[i].full = 0;
    }
    tLBA l_geom;
    g_idechs2lba(idechs, &l_geom);
    const string types[10]
            = {"Empty",
               "FAT12",
               "FAT16<32M",
               "Расширенный",
               "MS-DOS FAT16",
               "NTFS",
               "FAT32",
               "FAT16",
               "Linux swap",
               "Linux"};
    while (1) {
        if (tables[table_num].full >= 4) {
            break;
        }
        double size_to_create = 0;
        printf("Введите размер раздела: \n");
        scanf("%lf", &size_to_create);
        if (size_to_create == 0) {
            break;
        }
        if (free_size - size_to_create < 0) {
            printf("Раздел слишком большой.\n");
            continue;
        }
        free_size -= size_to_create;

        tables[table_num].records[tables[table_num].full].sectors
                = size_to_create * 1024 * 1024 / 512;
        printf("Выберите тип раздела: "
               "\n1.Empty\n2.FAT12\n3.FAT16<32M\n4.Расширенный\n5.MS-DOS "
               "FAT16\n6.NTFS\n7.FAT32\n8.FAT16\n9.Linux swap\n10.Linux\n");
        int32_t type = 0;
        scanf("%d", &type);
        while (type < 1 || type > 10) {
            printf("Неверное значение, повторите попытку: \n");
            scanf("%d", &type);
        }
        --type;
        tables[table_num].records[tables[table_num].full].type = types[type];
        if (!was_active) {
            printf("Сделать раздел активным? y/n\n");
            char c[10];
            scanf("%10s", c);
            if (c[0] == 'y') {
                was_active = 1;
                tables[table_num].records[tables[table_num].full].active = 1;
            }
        }
        tables[table_num].records[tables[table_num].full].l_start = now_at;
        tLBA tmp = now_at;
        now_at = now_at
                + tables[table_num].records[tables[table_num].full].sectors;
        tables[table_num].records[tables[table_num].full].end = now_at - 1;
        tCHS chs_geom;
        g_idechs2chs(idechs, &chs_geom);
        a_lba2chs(
                chs_geom,
                tables[table_num].records[tables[table_num].full].l_start,
                &tables[table_num].records[tables[table_num].full].chs_start);
        a_lba2chs(
                chs_geom,
                now_at - 1,
                &tables[table_num].records[tables[table_num].full].chs_end);
        ++tables[table_num].full;
        if (type == 3) {
            now_at = tmp + 2;
            ++table_num;
        }
    }
    printf("Активный Start           End          Секторы           Размер     "
           "  "
           "  Тип\n");
    for (uint64_t k = 0; k <= table_num; ++k) {
        for (uint64_t i = 0; i < tables[k].full; ++i) {
            if (tables[k].records[i].active == 1) {
                printf("*        ");
            } else {
                printf("         ");
            }
            printf("%-15d ", tables[k].records[i].l_start);
            printf("%-12d ", tables[k].records[i].end);
            printf("%-17d ", tables[k].records[i].sectors);
            printf("%-14d ", tables[k].records[i].sectors * 512);
            printf("%-4s\n", tables[k].records[i].type.c_str());
        }
    }

    return 0;
}
