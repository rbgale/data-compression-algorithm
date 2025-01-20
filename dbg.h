#include "pair.h"

/*
Write contents of table to a file.

Input:
    Pair *table: the table to read
    int len: the length of the table
    int c: the code specifier (0 for encode, 1 for decode)
*/
void dbg(Pair *table, int len, int c);

/*
Print table entries for debugging.

Input:
    Pair *table: the string table
    int len: the length of the table
    int all: set 1 to print children of entries; 0 otherwise
*/
void printTable(Pair *table, int len, int all);

/*
Print specific code table entries and all child entries for debugging.

Input:
    Pair *table: the string table
    int c: the code to print
*/
void printCode(Pair *table, int c);

