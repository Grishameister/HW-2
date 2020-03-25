#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

int main(int argc, const char **argv) {
    if (argc != 2) {
        return -1;
    }
    const char* path_to_text = argv[1];
    clock_t start_t, end_t;
    start_t = clock();
    char* max = NULL;
    parse_text(path_to_text, &max);
    end_t = clock();
    double total = (double)(end_t - start_t);
    total = total / CLOCKS_PER_SEC;
    printf("Total time taken by CPU: %f\n", total);
    free(max);
    return 0;
}
