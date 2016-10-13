#include <stdio.h>
#include <string.h>

#include "linkedlistz.h"

#define INT2VOIDP(i) (void*)(uintptr_t)(i)

int t = 0;

int parse_bytebeat(char *pattern)
{
    List *elementz = calloc(1, sizeof(List));
    list_init(elementz, NULL);

    char *c = pattern;
    while (*c++) {
        list_ins_next(elementz, NULL, INT2VOIDP(*c));
        printf("Charrrr %c\n", *c);
    }

    ListElmt *e;
    for ( e = elementz->head; e != NULL; e = e->next) {
        printf("ARF! %c\n", (char) list_data(e));
    }


    return 0;
}


int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Dingie mate - gimme a quote delimited bytebeat\n");
        return 1;
    }

    char pattern[128];
    int result = parse_bytebeat(strncpy(pattern, argv[1], 127));
    printf("\nRES: %d\n", result);

}
