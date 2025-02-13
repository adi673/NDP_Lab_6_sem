#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Comparison function for descending order (qsort uses this)
int compare_descending(const void *a, const void *b) {
    return (*(char *)b - *(char *)a);  // Reverse order
}

void sort_descending(char *str) {
    qsort(str, strlen(str), sizeof(char), compare_descending);
}

int main() {
    char str[] = "aB3cD1z2";

    printf("Original string: %s\n", str);

    // Sorting in descending order
    sort_descending(str);

    printf("Sorted string (descending): %s\n", str);
    return 0;
}

