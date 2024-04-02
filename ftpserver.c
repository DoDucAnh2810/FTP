/*
 * ftpserver.c - A pooled FTP server
 */

#include "csapp.h"
#include "utils.h"

static int nb_server_reaped = 0;

void sigchld_handler() {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) 
        nb_server_reaped++;
}

void sigint_handler() {
    kill(-getpid(), SIGTERM);
    while (nb_server_reaped < NB_PROC - 1);
    exit(1);
}

void sigpipe_handler() {
    printf("Detected that a client has crashed\n");
    fflush(stdout);
}

/* Return the working directory relative to the FTP server */
char *abstract_cwd(const char *root_path) {
    char raw_cwd[MAXLINE];
    getcwd(raw_cwd, sizeof(raw_cwd));
    char *result = strip_prefix(raw_cwd, root_path);
    if (strlen(result) == 0)
        strcat(result, "/");
    return result;
}

/* Change directory WITHIN the FTP server */
int cd(const char *root_path, const char *path) {
    // Concatenate the allowed directory path with the provided path
    char resolved_path[MAXLINE];
    if (realpath(path, resolved_path) == NULL)
        return 1;
    // Check if the resolved path starts with the allowed directory path
    if (strncmp(root_path, resolved_path, strlen(root_path)) != 0)
        return 1;
    // Change directory
    if (chdir(resolved_path) != 0)
        return 1;
    return 0;
}

/* Send the data from the file at target_path to the connection at conn_fd */
int ftp(int conn_fd, char *target_path) {
    struct stat file_stat;
    char buffer[MAXLINE];
    rio_t target_rio;
    int target_fd;
    size_t n;
    
    // Open target file and send diagnosis
    if (stat(target_path, &file_stat) == -1) {
        switch (errno) {
        case ENOENT:
            send_message(conn_fd, "No such file\n");
            break;
        case EACCES:
            send_message(conn_fd, "Permission denied\n");
            break;
        default:
            send_message(conn_fd, "Unknown error\n");
            break;
        }
        return 1;
    }
    if (!(file_stat.st_mode & S_IRUSR)) {
        send_message(conn_fd, "Permission denied\n");
        return 1;
    }
    if (S_ISDIR(file_stat.st_mode)) {
        send_message(conn_fd, "Is a directory\n");
        return 1;
    }
    if (strchr(target_path, '/')) {
        send_message(conn_fd, "File must be in current working directory\n");
        return 1;
    }

    send_message(conn_fd, "Successful request\n");
    target_fd = open(target_path, O_RDONLY, 0);

    // Send target file's size
    sprintf(buffer, "%lld\n", (long long)file_stat.st_size);
    send_message(conn_fd, buffer);

    // Send target file
    Rio_readinitb(&target_rio, target_fd);
    while ((n = Rio_readnb(&target_rio, buffer, MAXLINE)) != 0)
        if (rio_writen(conn_fd, buffer, n) == -1) {
            Close(target_fd);
            return 1;
        }

    // Terminating protocol
    Close(target_fd);
    send_message(conn_fd, "End\n");
    return 0;
}

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv) {
    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);
    Signal(SIGPIPE, sigpipe_handler);
    int listenfd, connfd, i, cmd_len, port, tube[2];
    char **cmd_line, root_path[MAXLINE], *cwd, *message;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    pid_t pid;
    rio_t conn_rio;
    int n;
    char buffer[MAXLINE];
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    getcwd(root_path, MAXLINE);
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);
    printf("Server is listening on port %d\n", port);

    for (i = 1; i < NB_PROC; i++)
        if (!(pid = Fork()))
            break;

    while (1) {
        // Round robin to get the connection
        while ((connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen)) < 0);

        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);
        
        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                INET_ADDRSTRLEN);
        
        printf("Server connected to (%s)\n", client_ip_string);

        // Loop through requests sent by client
        Rio_readinitb(&conn_rio, connfd);
        while ((n = Rio_readlineb(&conn_rio, buffer, MAXLINE)) != 0) {
            cmd_line = split_string_by_whitespace(buffer, &cmd_len);
            if (!cmd_len) {
                send_message(connfd, "Empty command line\n");
            } else if (are_equal_strings(cmd_line[0], "get")) {
                if (cmd_len < 2) {
                    send_message(connfd, "Empty get request\n");
                    continue;
                }
                printf("Received request for %s from (%s)\n", cmd_line[1], client_ip_string);
                if (ftp(connfd, cmd_line[1]))
                    printf("Failed request for %s from (%s)\n", cmd_line[1], client_ip_string);
                else
                    printf("Successful request for %s from (%s)\n", cmd_line[1], client_ip_string);
            } else if (are_equal_strings(cmd_line[0], "bye")) {
                send_message(connfd, "Goodbye!\n");
                break;
            } else if (are_equal_strings(cmd_line[0], "ls")) {
                if (cmd_len != 1) {
                    send_message(connfd, "Unsupported option/argument\n");
                    continue;
                }
                send_message(connfd, "Successful command\n");
                // Get the result of ls
                pipe(tube);
                if ((pid = Fork()) == 0) {
                    Dup2(tube[1], STDOUT_FILENO);
                    Close(tube[0]);
                    Close(tube[1]);
                    Close(connfd);
                    execlp("ls", "ls", NULL);
                    exit(1);
                }
                Close(tube[1]);
                n = Rio_readn(tube[0], buffer, MAXLINE);
                if (n <= 1) {
                    send_message(connfd, "ls: Empty directory\n");
                    continue;
                }
                // Edit and send the message
                for (int i = 0; i <= n-2; i++)
                    if (buffer[i] == '\n')
                        buffer[i] = ' ';
                buffer[n-1] = '\n';
                buffer[n] = '\0'; 
                send_message(connfd, buffer);
            } else if (are_equal_strings(cmd_line[0], "pwd")) {
                send_message(connfd, "Successful command\n");
                cwd = abstract_cwd(root_path);
                strcpy(buffer, cwd);
                strcat(buffer, "\n");
                send_message(connfd, buffer);
                free(cwd);
            } else if (are_equal_strings(cmd_line[0], "cd")) {
                if (cmd_len < 2) {
                    send_message(connfd, "Failed to change directory\n");
                    continue;
                }
                if (cd(root_path, cmd_line[1]))
                    send_message(connfd, "Failed to change directory\n");
                else {
                    send_message(connfd, "Successful command\n");
                    cwd = abstract_cwd(root_path);
                    message = add_prefix("Changed to directory: ", cwd);
                    strcpy(buffer, message);
                    strcat(buffer, "\n");
                    send_message(connfd,  buffer);
                    free(message);
                    free(cwd);
                }
            } else {
                send_message(connfd, "Unknown command\n");
            }
            free_string_array(cmd_line, cmd_len);
        }

        Close(connfd);
        printf("Closed connection to (%s)\n", client_ip_string);  
    }

    exit(0);
}

