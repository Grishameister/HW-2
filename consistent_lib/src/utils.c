#include <sched.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#include "utils.h"


int my_strncpy(const char* str, char** directory, size_t bytes) {
    if (!str) {
        return ERR_NULL;
    }
    *directory = calloc(bytes + 1, sizeof(char));
    if (!(*directory)) {
        return ERR_ALLOC;
    }
    size_t i = 0;
    while (i < bytes && str[i] != '\0') {
        (*directory)[i] = str[i];
        i++;
    }
    (*directory)[i] = '\0';
    return 0;
}


int my_strncat(char* first_str, const char* second_str, char** directory, size_t bytes) {
    if (!first_str || !second_str) {
        return ERR_NULL;
    }
    size_t first_size = strlen(first_str);

    *directory = calloc(first_size + bytes + 1, sizeof(char));
    if (!(*directory)) {
        return ERR_ALLOC;
    }

    size_t i = 0;
    while (first_str[i] != '\0') {
        (*directory)[i] = first_str[i];
        i++;
    }

    free(first_str);
    size_t k = 0;
    while (k < bytes && second_str[k] != '\0') {
        (*directory)[i] = second_str[k];
        i++;
        k++;
    }
    (*directory)[i] = '\0';
    return 0;
}


static int is_new_sequence(const char symbol) {
    return 'A' <= symbol && symbol <= 'Z';
}

static char* strchr_for_part(const char* str, char symbol, size_t size) {
    if (!str) {
        return NULL;
    }
    size_t i = 0;
    while (i < size && str[i]) {
        if (str[i] == symbol) {
            break;
        }
        i++;
    }
    if (str[i] == symbol && i < size) {
        char* new_string = (char*)str;
        new_string += i;
        return new_string;
    }
    return NULL;
}

int find_max_string(const char* text, char** directory, size_t size_of_part) {
    if (!text) {
        return ERR_NULL;
    }

    char* new_string = NULL;
    size_t max_size = 0;
    char* start_ptr = strchr_for_part(text, '"', size_of_part);
    if (!start_ptr) {
        return ERR_NULL;
    }
    int flag = 0;

    char* current_ptr = start_ptr;
    size_t size = 0;
    size_t i = current_ptr - text;
    while (current_ptr) {
        if (flag) {
            size = current_ptr - start_ptr;
        }

        if (size > max_size) {
            max_size = size;
            new_string = start_ptr;
        }
        if (i >= size_of_part) {
            break;
        }
        if (is_new_sequence(current_ptr[1])) {
            flag = 1;
            start_ptr = current_ptr + 1;
        } else {
            flag = 0;
        }
        current_ptr = strchr(current_ptr + 1, '"');
        if (!current_ptr) {
            break;
        }
        i = current_ptr - text;
    }

    if (!new_string || !max_size) {
        return ERR_NULL;
    }

    if (my_strncpy(new_string, directory, max_size)) {
        return ERR_COPY;
    }
    return 0;
}


int parse_text(const char* path_to_text, char** directory) {
    FILE* text = fopen(path_to_text, "r");
    if (!text) {
        return ERR_OPEN_FILE;
    }
    int descr = fileno(text);
    struct stat st;
    if (fstat(descr, &st) != 0) {
        fclose(text);
        return ERR_DSCR;
    }

    char* ptr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, descr, 0);
    if (ptr == MAP_FAILED) {
        fclose(text);
        return ERR_MMAP;
    }

    if (find_max_string(ptr, directory, st.st_size)) {
        return ERR_FIND_STR;
    }
    munmap(ptr, st.st_size);
    fclose(text);
    return 0;
}
