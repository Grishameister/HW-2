#include "gtest/gtest.h"

extern "C" {
#include "utils.h"
}

TEST(CopyString, NullString) {
    const char* null_string = NULL;
    char* directory = NULL;
    size_t bytes = 5;
    EXPECT_EQ(my_strncpy(null_string, &directory, bytes), ERR_NULL);
    free(directory);
}

TEST(CopyString, ValidString) {
    const char valid_string[] = "This is string!#";
    size_t bytes = strlen(valid_string);
    char* directory = NULL;

    EXPECT_EQ(my_strncpy(valid_string, &directory, bytes), SUCCESS);
    EXPECT_STREQ(valid_string, directory);
    free(directory);
}

TEST(ConcatinateStrings, NullString) {
    char string[] = "123";
    const char* null_string = NULL;
    char* directory = NULL;
    size_t bytes = 5;

    EXPECT_EQ(my_strncat(string, null_string, &directory, bytes), ERR_NULL);
    free(directory);
}

TEST(ConcatinateStrings, ValidString) {
    char string[] = "user@mail.ru";
    char* alloc_string = NULL;

    my_strncpy(string, &alloc_string, strlen(string));

    const char address[] = "doparse@headhunter.ru";
    char result[] = "user@mail.rudoparse";
    char* directory = NULL;
    size_t bytes = 7;

    EXPECT_EQ(my_strncat(alloc_string, address, &directory, bytes), SUCCESS);

    EXPECT_STREQ(result, directory);
    free(directory);
}

TEST(FindMaxStr, NullString) {
    const char* null_string = NULL;
    char* directory = NULL;
    size_t size_of_part = 12;

    EXPECT_EQ(find_max_string(null_string, &directory, size_of_part), ERR_NULL);
    free(directory);
}


TEST(FindMaxStr, NoQuotes) {
    const char string[] = "Technopark2020spring";
    char* directory = NULL;
    size_t size_of_part = strlen(string);

    EXPECT_EQ(find_max_string(string, &directory, size_of_part), ERR_NULL);
    free(directory);
}

TEST(FindMaxStr, NoSubstring) {
    const char string[] = {'T', 'e', 'c', 'h', 'n', 'o', 'p', 'a', '"', '"', 'r', 'k', '2', '\0'};
    char* directory = NULL;
    size_t size_of_part = 13;

    EXPECT_EQ(find_max_string(string, &directory, size_of_part), ERR_NULL);
    free(directory);
}

TEST(FindMaxStr, ValidInfo) {
    const char* path_to_file = "../test/data/test.eml";
    char* directory = NULL;
    EXPECT_EQ(parse_text(path_to_file, &directory), SUCCESS);
    free(directory);
}

TEST(FindMaxStr, ValidInfoCompare) {
    const char* path_to_file = "../test/data/btests_emails_broken-ctype.eml";
    const char* answer = "Should make its way back into";
    char* directory = NULL;
    EXPECT_EQ(parse_text(path_to_file, &directory), 0);
    EXPECT_STREQ(directory, answer);
    free(directory);
}

TEST(FindMaxStr, ValidInfoCount) {
    const char* path_to_file = "../test/data/btests_emails_big.eml";
    char* directory = NULL;
    size_t answer = 55744;
    EXPECT_EQ(parse_text(path_to_file, &directory), SUCCESS);
    EXPECT_EQ(strlen(directory), answer);
    free(directory);
}