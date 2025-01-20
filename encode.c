#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "encode.h"
#include "dbg.h"
#include "prune.h"

#define MIN(x, y) (x < y ? x : y)

static void put(int c, int *packed_byte, int *bit_offset, int code_size);

void encode(int p, int max_bit_length) {
    // Define constants
    const int PRUNE = p;
    const int MAX_BITS = max_bit_length;
    const int TABLE_SIZE = 1 << MAX_BITS;
    
    // Print max bits and pruning value (packed in binary)
    putchar((MAX_BITS << 1) + PRUNE);

    // Create table
    Pair *table = malloc(TABLE_SIZE * sizeof(Pair));

    if (NULL == table) {
        fprintf(stderr, "Error: insufficient memory to allocate string table.\n");
        exit(1);
    } 

    // Initialize table to contain (EMPTY, char) pairs
    // (Characters are stored by ASCII integer value; -1 represents EMPTY) 
    for (int i = 0; i < 257; i++)
        table[i] = (Pair) {0, i - 1};

    // BEGIN ENCODE

    // 0-256 are single characters
    // 257 reserved for prune signal
    // 258 reserved for code size change
    const int PRUNE_SIGNAL = 257;
    const int SIZE_CHANGE = 258;

    int c = 0;
    int k = 0;
    int i = 259;
    int start = i;

    // Variables for bit packing
    int packed_byte = 0;
    int bit_offset = 0;
    int code_size = 9;

    while ((k = fgetc(stdin)) != EOF) {
        // Find code in table (-1 if not contained)
        int code = -1;
        
        // Skip lookup if code is a single character
        if (c == 0) {
            code = k + 1;
        } else {
            for (int j = 0; j < i; j++) { 
                if (table[j].c == c && table[j].k == k) {
                    code = j;
                    break;
                }
            }
        }

        if (code == -1) {
            // Print prefix
            put(c, &packed_byte, &bit_offset, code_size);

            // Add new code to table if not full
            if (i < TABLE_SIZE) {
                table[i++] = (Pair) {c, k};
            
                // Send signal to change size if necessary
                if (i >= (1 << code_size)) {
                    put(SIZE_CHANGE, &packed_byte, &bit_offset, code_size);
                    code_size++;
                }
            // Prune table if full
            } else if (PRUNE) {
                // Send signal to tell decode to prune
                put(PRUNE_SIGNAL, &packed_byte, &bit_offset, code_size);
                i = prune(table, TABLE_SIZE, start);
                
                // Reset code size
                int temp = i;
                code_size = 1;
                while ((temp >>= 1) > 0)
                    code_size++;
            }

            // Set prefix to code of current char
            c = k + 1;
        } else {
            // Set prefix to current code
            c = code;
        }
    }

    if (c != 0) {
        put(c, &packed_byte, &bit_offset, code_size);
    }

    // Print remainder of file
    putchar(packed_byte);

    // If DBG variable is set, print table
    dbg(table, i, 0);

    // Free table
    free(table);
}

/*
Prints a code to stdout. Prints in binary format if stage is 3.

Input:
    int c: the code to output
    int *packed_byte: the current byte being packed
    int *bit_offset: the first unwritten bit of the byte
    int code_size: the current code size 
 */
static void put(int c, int *packed_byte, int *bit_offset, int code_size) {
    int num_packed = 0;
    while (num_packed != code_size) {
        // Determine bits to extract and retrieve them from the code
        int num_bits = MIN(8 - *bit_offset, code_size - num_packed);
        int segment = (c >> num_packed) & ((1 >> num_bits) - 1);

        // Insert bits into packed byte
        *packed_byte += segment << *bit_offset;
        *bit_offset = (*bit_offset + num_bits) % 8;

        // If offset is 0, the whole byte has been written
        if (*bit_offset == 0) {
            putchar(*packed_byte);
            *packed_byte = 0;
        }

        // Update amount of bits packed
        num_packed += num_bits;
    }
}

