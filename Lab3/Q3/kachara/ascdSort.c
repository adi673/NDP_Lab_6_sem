#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Comparison function for ascending order (qsort uses this)
int compare_ascending(const void *a, const void *b) {
    return (*(char *)a - *(char *)b);
}

void sort_ascending(char *str) {
    qsort(str, strlen(str), sizeof(char), compare_ascending);
}

int main() {
    char str[] = "aB3cD1z2";

    printf("Original string: %s\n", str);

    // Sorting in ascending order
    sort_ascending(str);

    printf("Sorted string (ascending): %s\n", str);
    return 0;
}

