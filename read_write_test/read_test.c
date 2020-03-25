#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#include "utils.h"

#define ERR_PARSE -9
#define ERR_TEST -10

int main(int argc, const char **argv) {
    if (argc != 2) {
        return -1;
    }
    const char* path_to_text = argv[1];

    clock_t start_t, end_t;
    start_t = clock();

    char* max = NULL;
    if (parse_text(path_to_text, &max)) {
        free(max);
        return ERR_PARSE;
    }

    end_t = clock();
    double total = (double)(end_t - start_t);
    total = total / CLOCKS_PER_SEC;

    if (!max) {
        return ERR_NULL;
    }

    FILE* test_file = fopen("../test_file.txt", "r");
    if (!test_file) {
        return ERR_OPEN_FILE;
    }

    int descr = fileno(test_file);
    struct stat st;
    if (fstat(descr, &st) != 0) {
        fclose(test_file);
        return ERR_DSCR;
    }

    FILE* compare_time = fopen("../compare_time.txt", "r");
    if (!compare_time) {
        fclose(test_file);
        free(max);
        return ERR_TEST;
    }

    double first_time = 0;

    if (fscanf(compare_time, "%lf", &first_time) != 1) {
        fclose(test_file);
        fclose(compare_time);
        free(max);
        return ERR_TEST;
    }

    char* ptr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, descr, 0);
    if (ptr == MAP_FAILED) {
        fclose(compare_time);
        fclose(test_file);
        free(max);
        return ERR_MMAP;
    }

    if (strcmp(max, ptr)) {
        fclose(compare_time);
        fclose(test_file);
        free(max);
        munmap(ptr, st.st_size);
        printf("TEST FAILED\n");
        return ERR_TEST;
    }

    munmap(ptr, st.st_size);
    printf("First time is %lf\n", first_time);
    printf("Second time is %lf\n", total);
    printf("TEST SUCCESS\n");
    free(max);
    fclose(compare_time);
    fclose(test_file);
    return 0;
}
