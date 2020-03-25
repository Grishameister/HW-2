#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include "utils.h"

#define ERR_PARSE -9

int main(int argc, const char **argv) {
    if (argc != 2) {
        return -1;
    }
    const char* path_to_text = argv[1];
    FILE* test_file = fopen("../test_file.txt", "w");
    if (!test_file) {
        return ERR_OPEN_FILE;
    }
    int fd = fileno(test_file);
    clock_t start_t, end_t;
    start_t = clock();
    char* max = NULL;

    if (parse_text(path_to_text, &max)) {
        free(max);
        fclose(test_file);
        return ERR_PARSE;
    }
    end_t = clock();
    double total = (double)(end_t - start_t);
    total = total / CLOCKS_PER_SEC;
    if (!max) {
        fclose(test_file);
        return ERR_NULL;
    }
    write(fd, max, strlen(max) + 1);
    FILE* compare_time = fopen("../compare_time.txt", "w");
    fprintf(compare_time, "%f\n", total);
    free(max);
    fclose(test_file);
    fclose(compare_time);
    return 0;
}
