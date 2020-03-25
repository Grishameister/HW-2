#ifndef CONSISTENT_LIB_INCLUDE_UTILS_H_
#define CONSISTENT_LIB_INCLUDE_UTILS_H_

enum Error_code {
    SUCCESS = 0,
    ERR_OPEN_FILE = -1,
    ERR_DSCR = -2,
    ERR_MMAP = -3,
    ERR_BNDR = -4,
    ERR_NULL = -5,
    ERR_ALLOC = -6,
    ERR_COPY = -7,
    ERR_FIND_STR = -8
};

int my_strncpy(const char* str, char** directory, size_t bytes);
int my_strncat(char* first_str, const char* second_str, char** directory, size_t bytes);

int find_max_string(const char* text, char** directory, size_t size_of_part);

int parse_text(const char* path_to_text, char** directory);


#endif  // CONSISTENT_LIB_INCLUDE_UTILS_H_
