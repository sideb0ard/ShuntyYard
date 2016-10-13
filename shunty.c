#include <stdio.h>
#include <string.h>

int t = 0;

int parse_bytebeat(char *pattern)
{
    char *c = pattern;
    while (*c++) {
        printf("Charrrr %c\n", *c);
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
    printf("RES: %d\n", result);
}
