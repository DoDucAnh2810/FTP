#ifndef __UTILS_H__
#define __UTILS_H__

#define PACKAGE_SIZE 8192
#define SERVER_PORT 2121
#define MAX_NAME_LEN 256
#define MAX_WORDS 4096
#define NB_PROC 1

/* Send message to the connection using fd */
void send_message(int fd, char *message);

/* Check if two strings are equal */
int are_equal_strings(char *str1, char *str2);

char **split_string_by_whitespace(const char* input_string, int* num_words);

#endif