#include <stdio.h>
#include <stdlib.h>
#include "pair.h"

int prune(Pair *table, int len, int start) {
    // Contains 1 at indices whose code will survive pruning
    // and 0 at all indices that will be removed from the table
    int *save = calloc((len), sizeof(int));

    // Contains amounts to shift each index, if used as a prefix
    int *shift = calloc((len), sizeof(int));
    
    if (NULL == save || NULL == shift) {
        fprintf(stderr, "Error: insufficient memory to allocate array.\n");
        exit(1);
    }

    for (int i = start; i < len; i++) {
        if (table[i].c >= start) {
            save[table[i].c] = 1;
        }
    }

    for (int i = start; i < len; i++)
        shift[i] = shift[i - 1] + (1 - save[i]);

    // Prune all non-saved codes
    int index = start;
    for (int i = start; i < len; i++) {
        if (save[i])
            table[index++] = (Pair) {table[i].c - shift[table[i].c], table[i].k};
    }

    return index;
}

