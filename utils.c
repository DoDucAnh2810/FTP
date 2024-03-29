/*
 * utils.c - Utilities for FTP
 */

#include <string.h>
#include "csapp.h"

void send_message(int fd, char *message) {
    Rio_writen(fd, message, strlen(message));
}

int are_equal_strings(char *str1, char *str2) {
    return strcmp(str1, str2) == 0;
}