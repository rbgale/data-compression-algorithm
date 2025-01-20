#include <stdio.h>
#include <stdlib.h>
#include "decode.h"
#include "dbg.h"
#include "prune.h"

#define MIN(x, y) (x < y ? x : y)

static void parseEncode(int *mb, int *p);
static int get(int *packed_byte, int *bit_offset, int code_size);

void decode() {
    // Determine max bits and pruning status
    int mb, p;
    parseEncode(&mb, &p);

    // Define constants
    const int PRUNE = p;
    const int MAX_BITS = mb;
    const int TABLE_SIZE = 1 << MAX_BITS;

    // Create table
    Pair *table = malloc(TABLE_SIZE * sizeof(Pair));

    if (NULL == table) {
        fprintf(stderr, "Error: insufficient memory to allocate string table.\n");
        exit(1);
    }

    // Initialize table to contain (EMPTY, char) pairs
    // (Characters are stored by ASCII integer value and -1 represents EMPTY) 
    for (int i = 0; i < 257; i++)
        table[i] = (Pair) {0, i - 1};

    // BEGIN DECODE

    // 0-256 are single characters
    // 257 reserved for prune signal
    // 258 reserved for code size change
    const int PRUNE_SIGNAL = 257;
    const int SIZE_CHANGE = 258;

    int oldC = 0;
    int c = 0;
    int i = 259;
    int start = i;
    
    // Create character stack
    int top = -1;
    int finalK = 0;
    int *kStack = malloc(TABLE_SIZE * sizeof(int));

    if (NULL == kStack) {
        fprintf(stderr, "Error: insufficient memory to allocate character stack.\n");
        exit(1);
    }

    // Variables for bit unpacking
    int packed_byte = -2;
    int bit_offset = 0;
    int code_size = 9;

    while ((c = get(&packed_byte, &bit_offset, code_size)) != EOF) { 
        int newC = c;

        // Check for prune signal
        if (c == PRUNE_SIGNAL && PRUNE) {
            i = prune(table, TABLE_SIZE, start);
            
            // Reset code size
            int temp = i;
            code_size = 1;
            while ((temp >>= 1) > 0)
                code_size++;
            
            // Reset prefix
            oldC = 0; 
            
            continue;
        }

        // Check for size change
        if (c == SIZE_CHANGE) {
            code_size++;
            continue;
        }

        if (c >= i) {  
            kStack[++top] = finalK;
            c = oldC;
        }

        while (table[c].c != 0) {
            kStack[++top] = table[c].k;
            c = table[c].c;
        }

        finalK = table[c].k;
        printf("%c", finalK);

        while (top > -1)
            printf("%c", kStack[(top--)]);

        if (oldC != 0 && i < TABLE_SIZE)
            table[i++] = (Pair) {oldC, finalK};

        oldC = newC;
    }
    
    free(kStack);
 
    // If DBG variable is set, print table
    dbg(table, i, 1);

    // Free table
    free(table);
}

/*
Reads the first two line of output from encode and stores them in out
parameters for max bits and pruning information.

Input:
    int *mb: out parameter for max bits
    int *p: out parameter for pruning (1 if enabled; 0 otherwise) 
*/
static void parseEncode(int *mb, int *p) {
    char buffer[10], *end;
    int max_bits = 12;
    int prune = 0;

    int byte = getchar();
    if (byte != EOF) {
        max_bits = byte >> 1;
        prune = byte & 1;
    }

    *mb = max_bits;
    *p = prune;
}

/*
Reads a code from stdin in binary format.

Input:
    int *packed_byte: the current byte being packed
    int *bit_offset: the first unwritten bit of the byte
    int code_size: the current code size 
 */
static int get(int *packed_byte, int *bit_offset, int code_size) {
    if (*packed_byte == -2)
        *packed_byte = getchar();

    if (*packed_byte == EOF)
        return EOF;

    int code = 0;
    int num_unpacked = 0;
    while (*packed_byte != EOF && num_unpacked != code_size) {
        int num_bits = MIN(8 - *bit_offset, code_size - num_unpacked);

        int segment = (*packed_byte >> *bit_offset) & ((1 << num_bits) - 1);
        code += segment << num_unpacked;

        *bit_offset = (*bit_offset + num_bits) % 8;

        num_unpacked += num_bits;

        if (*bit_offset == 0)
            *packed_byte = getchar();
    }

    // Final code is nonsense if not enough bits were read
    if (*packed_byte == EOF && num_unpacked != code_size)
        return EOF;
    return code;
}

