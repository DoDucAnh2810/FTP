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
        fprintf(stderr, "Fatal: Memory allocation failed\n");
        exit(1);
    }

    *num_words = 0;
    token = strtok(copy, " \t\n");
    while (token != NULL) {
        words[*num_words] = strdup(token);
        if (words[*num_words] == NULL) {
            fprintf(stderr, "Fatal: Memory allocation failed\n");
            exit(1);
        }
        (*num_words)++;
        token = strtok(NULL, " \t\n");
    }

    free(copy);
    return words;
}

void free_string_array(char** words, int num_words) {
    for (int i = 0; i < num_words; i++) {
        free(words[i]);
    }
    free(words);
}

cluster_t *parse_cluster_list(char *filepath, int *nb_cluster) {
    // Get the file size
    struct stat file_stat;
    if (stat(filepath, &file_stat) == -1 || !(file_stat.st_mode & S_IRUSR)) {
        fprintf(stderr, "Fatal: Cannot read data on clusters\n");
        exit(1);
    }
    long file_size = file_stat.st_size;
    
    // Allocate memory to buffer
    char *jsonBuffer = (char *)malloc(file_size + 1);
    if (jsonBuffer == NULL) {
        fprintf(stderr, "Fatal: Memory allocation failed.\n");
        exit(1);
    }

    // Read data into buffer
    int fd = Open(filepath, O_RDONLY, 0);
    Rio_readn(fd, jsonBuffer, file_size);
    Close(fd);
    jsonBuffer[file_size] = '\0';

    // Parse json
    cJSON *object_list = cJSON_Parse(jsonBuffer);
    free(jsonBuffer);

    // Check if data is well defined
    if (object_list == NULL || !cJSON_IsArray(object_list)) {
        cJSON_Delete(object_list);
        fprintf(stderr, "Fatal: Cannot read data on clusters\n");
        exit(1);
    }

    // Allocate memory for resulting array
    *nb_cluster = cJSON_GetArraySize(object_list);
    cluster_t *cluster_list = (cluster_t *)malloc(sizeof(cluster_t) * (*nb_cluster));
    if (cluster_list == NULL) {
        cJSON_Delete(object_list);
        fprintf(stderr, "Fatal: Memory allocation failed.\n");
        exit(1);
    }

    // Loop through the list
    for (int i = 0; i < *nb_cluster; i++) {
        cJSON *object = cJSON_GetArrayItem(object_list, i);
        cJSON *name = cJSON_GetObjectItem(object, "name");
        cJSON *host = cJSON_GetObjectItem(object, "host");
        cJSON *port = cJSON_GetObjectItem(object, "port");
        cluster_t cluster;
        cluster.name = strdup(name->valuestring);
        cluster.host = strdup(host->valuestring);
        cluster.port = port->valueint;
        if (cluster.name == NULL || cluster.host == NULL) {
            cJSON_Delete(object_list);
            free(cluster_list);
            fprintf(stderr, "Fatal: Memory allocation failed.\n");
            exit(1);
        }
        cluster_list[i] = cluster;
    }

    return cluster_list;
}


// Function to strip a given prefix from a given string
char *strip_prefix(const char *string, const char *prefix) {
    size_t prefix_length = strlen(prefix);

    // Check if the string starts with the prefix
    if (strncmp(string, prefix, prefix_length) == 0) {
        // Move the string pointer forward by the length of the prefix
        return (char *)strdup(string + prefix_length);
    } else {
        // If the prefix is not found, return the original string
        return (char *)strdup(string);
    }
}

char* add_prefix(const char* prefix, const char* buffer) {
    // Allocate memory for the new string
    size_t prefix_len = strlen(prefix);
    size_t buffer_len = strlen(buffer);
    char* result = (char*)malloc(prefix_len + buffer_len + 1); // +1 for null terminator

    if (result == NULL) {
        fprintf(stderr, "Fatal: Memory allocation failed.\n");
        exit(1);
    }

    // Copy the prefix and the buffer into the new string
    sprintf(result, "%s%s", prefix, buffer);

    return result;
}