/*
 * utils.c - Utilities for FTP
 */

#include <string.h>
#include "csapp.h"
#include "utils.h"

void send_message(int fd, char *message) {
    Rio_writen(fd, message, strlen(message));
}

int are_equal_strings(char *str1, char *str2) {
    return strcmp(str1, str2) == 0;
}

char **split_string_by_whitespace(const char* input_string, int* num_words) {
    char** words = (char**)malloc(MAX_WORDS * sizeof(char*));
    if (words == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    char* token;
    char* copy = strdup(input_string);
    if (copy == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    *num_words = 0;
    token = strtok(copy, " \t\n");
    while (token != NULL) {
        words[*num_words] = strdup(token);
        if (words[*num_words] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        (*num_words)++;
        token = strtok(NULL, " \t\n");
    }

    free(copy);
    return words;
}
