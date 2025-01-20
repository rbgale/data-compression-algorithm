/*
Removes all non-single character entries in the table that are not the prefix
of any other codes.

Input:
    Pair *table: the string table
    int len: the length of the table
    int start: the first non-single character index

Output:
    Returns the least index of the table not yet written to, after pruning.
*/
int prune(Pair *table, int len, int start);

