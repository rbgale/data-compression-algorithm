#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h> // Include for basename
#include <limits.h>

#include "encode.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    static char bin[64], bout[64];
    setvbuf(stdin, bin, _IOFBF, 64);
    setvbuf(stdout, bout, _IOFBF, 64);

    char *exec_name = basename(argv[0]); // Get the executable name

    if (strcmp(exec_name, "encode") == 0) {
        int maxBits = 12;
        int pruning = 0;

        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-m") == 0) {
                if (argc == i + 1) {
                    fprintf(stderr, "encode: max bits not specified\n");
                    exit(1);
                }

                // Get int value from string argument
                const char *bits = argv[i + 1];
                char *end;
                maxBits = strtol(argv[i + 1], &end, 10);

                // Check that max bits is in range and argument is a number
                if (maxBits <= CHAR_BIT || maxBits > (2 * CHAR_BIT + 4) || *end != '\0') {
                    fprintf(stderr, "encode: invalid option '%s'\n", argv[i + 1]);
                    exit(1);
                }

                // Increment index because next argument has been read
                i++;
            } else if (strcmp(argv[i], "-p") == 0) {
                pruning = 1;
            } else {
                fprintf(stderr, "encode: invalid option '%s'\n", argv[i]);
                exit(1);
            }
        }
        
        encode(pruning, maxBits);
    } else if (strcmp(exec_name, "decode") == 0) {
        if (argc > 1) {
            fprintf(stderr, "decode: invalid option '%s'\n", argv[1]);
            exit(1);
        }
        decode();
    } else {
        fprintf(stderr, "Usage: %s [-m MAXBITS] [-p] < input > output\n", argv[0]);
        fprintf(stderr, "       %s < input > output\n", argv[0]);
        exit(1);
    }

    exit(0);
}
