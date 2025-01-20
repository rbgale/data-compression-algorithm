#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) { 
    if (argc > 2) {
        fprintf(stderr, "Error: too many arguments.\n");
        exit(1);
    }

    long n = 1000000;

    if (argc == 2) {
        char *end;
        n = strtol(argv[1], &end, 10);

        if (end == argv[1] || *end != '\0') {
            fprintf(stderr, "Error: argument '%s' must be an integer.\n", argv[1]);
            exit(1);
        }
    }

    FILE *fout = fopen("letters.txt", "w");
    if (NULL == fout) {
        fprintf(stderr, "Error: unable to open file.\n");
    }

    for (int i = 0; i < n; i++) {
        fprintf(fout, "%c", 'A');
    }

    fprintf(fout, "\n");
    fclose(fout);

    return 0;
}
