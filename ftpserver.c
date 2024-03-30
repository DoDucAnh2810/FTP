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
    kill(-getpid(), SIGINT);
    while (nb_server_reaped < NB_PROC - 1);
    exit(1);
}

void sigpipe_handler() {
    printf("Detected that a client has crashed\n");
    fflush(stdout);
}

/* Send the data from the file at target_path to the connection at conn_fd */
int ftp(int conn_fd, char *target_path) {
    struct stat file_stat;
    char buffer[MAXLINE];
    rio_t target_rio;
    int target_fd;
    size_t n;
    
    // Open target file and send diagnosis
    target_fd = open(target_path, O_RDONLY, 0);
    if (target_fd == -1) {
        switch (errno) {
        case ENOENT:
            send_message(conn_fd, "No such file\n");
            break;
        case EACCES:
            send_message(conn_fd, "Permission denied\n");
            break;
        case EISDIR:
            send_message(conn_fd, "Is a directory\n");
            break;
        case ENAMETOOLONG:
            send_message(conn_fd, "Filename too long\n");
            break;
        default:
            send_message(conn_fd, "Unknown error\n");
            break;
        }
        return 1;
    }
    send_message(conn_fd, "Successful request\n");

    // Send target file's size
    stat(target_path, &file_stat);
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
    int listenfd, connfd, i, cmd_len, port;
    char **cmd_line;
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

