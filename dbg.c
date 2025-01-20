#include <stdio.h>
#include <stdlib.h>
#include "dbg.h"

void dbg(Pair *table, int len, int c) {
    const char *debug = getenv("DBG");
    char *end = "";

    // If debug is NULL, no debug was specified
    int DBG = (NULL == debug) ? 0 : strtol(debug, &end, 10);

    // Throw error if debug is not a number
    if (*end != '\0') {
        fprintf(stderr, "Error: invalid debug value.\n");
        exit(1);
    }

    // Output file if debug is not set to 0
    if (DBG == 0)
        return;

    // Create output file
    FILE *fout;
    
    // Determine file name based on input specifier
    if (c) {
        fout = fopen("DBG.decode", "w");
    } else {
        fout = fopen("DBG.encode", "w");
    }

    if (fout == NULL) {
        fprintf(stderr, "Error: unable to open file.\n");
        exit(1);
    }

    // Write to file
    fprintf(fout, "Index: (prefix, char)\n");

    for (int i = 0; i < len; i++) {
        fprintf(fout, "%d: (%d, %d)\n", i, table[i].c, table[i].k);
    }

    // Close file
    fclose(fout);
}

void printTable(Pair *table, int len, int all) {
    printf("--------Table--------\n");
    for (int j = 257; j < len; j++) {
        if (all) {
            printCode(table, j);
        } else {
            printf("%d: (%d, %d)\n", j, table[j].c, table[j].k);
        }
    }
    printf("---------------------\n");
}

void printCode(Pair *table, int c) {
    printf("%d: ", c);
    printf("(%d, %d) ", table[c].c, table[c].k);
    do {
        printf("-> (%d, %d) ", table[c].c, table[c].k);
        c = table[c].c;
    } while (c > 0);
    printf("\b\n");
}

