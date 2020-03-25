#define _GNU_SOURCE
#include <sched.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils.h"

#define ERR_FORK -11
#define MAX_PIPE_SIZE 1000000

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


static int buffer_has_end_of_str(char* str, size_t* num, size_t bytes) {
    if (!str) {
        return ERR_NULL;
    }

    size_t i = 0;
    while (i < bytes && str[i] != '\0') {
        i++;
    }

    if (i < bytes && str[i] == '\0') {
        *num = i;
        return 1;
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

    int fd[2];
    pid_t pid;
    pipe(fd);
    fcntl(fd[1], F_SETPIPE_SZ, MAX_PIPE_SIZE);
    fcntl(fd[0], F_SETPIPE_SZ, MAX_PIPE_SIZE);

    int num_cpu = sysconf(_SC_NPROCESSORS_ONLN);

    size_t part = st.st_size / num_cpu;
    size_t offset = 0;
    int error_code = 0;
    int i = 0;
    while (i < num_cpu) {
        pid = fork();
        if (pid < 0) {
            return ERR_FORK;
        }
        if (pid == 0) {
            close(fd[0]);
            char* max = NULL;
            int a = find_max_string(ptr + offset, &max, part);
            if (a) {
                close(fd[1]);
                munmap(ptr, st.st_size);
                fclose(text);
                free(max);
                exit(ERR_FIND_STR);
            }
            size_t size_str = strlen(max);
            write(fd[1], max, size_str + 1);
            close(fd[1]);
            munmap(ptr, st.st_size);
            fclose(text);
            free(max);
            exit(0);
        }
        if (pid > 0) {
            i++;
            offset += part;
        }
    }

    if (pid > 0) {
        close(fd[1]);
        char* buffer = calloc(60000, sizeof(char));
        size_t size = 60000;
        size_t size_read = 0;

        for (int i = 0; i < num_cpu; i++) {
            wait(NULL);
        }

        int in_string = 0;
        char* temp_str = NULL;
        char* max_str = NULL;
        while ((size_read = read(fd[0], buffer, size)) > 0) {
            i++;
            size_t num = 0;
            size_t offset = 0;
            while (offset < size_read && buffer_has_end_of_str(buffer + offset, &num, size_read - offset)) {
                if (in_string) {
                    if (my_strncat(temp_str, buffer + offset, &temp_str, size_read - offset)) {
                        free(max_str);
                        free(temp_str);
                        error_code = ERR_COPY;
                        break;
                    }
                    in_string = 0;
                    if (max_str == NULL || strlen(max_str) < strlen(temp_str)) {
                        free(max_str);
                        max_str = temp_str;
                    } else {
                        free(temp_str);
                    }
                } else {
                    if (my_strncpy(buffer + offset, &temp_str, size_read - offset)) {
                        free(max_str);
                        free(temp_str);
                        error_code = ERR_COPY;
                        break;
                    }

                    in_string = 0;
                    if (max_str == NULL || strlen(max_str) < strlen(temp_str)) {
                        free(max_str);
                        max_str = temp_str;
                    } else {
                        free(temp_str);
                    }
                }
                offset += num + 1;
            }
            if (in_string) {
                if (my_strncat(temp_str, buffer + offset, &temp_str, size_read - offset)) {
                    free(max_str);
                    free(temp_str);
                    error_code = ERR_COPY;
                    break;
                }
            } else {
                if (my_strncpy(buffer + offset, &temp_str, size_read - offset)) {
                    free(max_str);
                    free(temp_str);
                    error_code = ERR_COPY;
                    break;
                }
                in_string = 1;
            }
        }
        close(fd[0]);
        munmap(ptr, st.st_size);
        *directory = max_str;
        free(buffer);
        free(temp_str);
        fclose(text);
    }
    return error_code;
}
