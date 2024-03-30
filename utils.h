#ifndef __UTILS_H__
#define __UTILS_H__

#include "cJSON.h"

#define PACKAGE_SIZE 8192
#define MASTER_PORT 2121
#define MAX_NAME_LEN 256
#define MAX_WORDS 4096
#define NB_PROC 2

typedef struct {
    char *name;
    char *host;
    int port;
} cluster_t;

/* Send message to the connection using fd */
void send_message(int fd, char *message);

/* Check if two strings are equal */
int are_equal_strings(char *str1, char *str2);

/* Split string into an array of string based on white space
   Put the number of word after split into num_words */
char **split_string_by_whitespace(const char* input_string, int* num_words);

/* Free an array of string */
void free_string_array(char** words, int num_words);

cluster_t *parse_cluster_list(char *filepath, int *nb_cluster);

#endif