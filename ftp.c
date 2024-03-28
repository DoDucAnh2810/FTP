/*
 * echo - read and echo text lines until client closes connection
 */
#include <sys/stat.h>
#include "csapp.h"
#include "string.h"

void send_message(int fd, char *message) {
    Rio_writen(fd, message, strlen(message));
}

void ftp(int conn_fd, char *target_path)
{
    size_t n;
    int target_fd;
    char buffer[MAXLINE];
    struct stat file_stat;
    rio_t conn_rio, target_rio;
    
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
        return;
    }

    stat(buffer, &file_stat);
    sprintf(buffer, "%lld\n", (long long)file_stat.st_size);
    send_message(conn_fd, "Success\n");
    send_message(conn_fd, buffer);
    Rio_readinitb(&target_rio, target_fd);

    while ((n = Rio_readnb(&target_rio, buffer, MAXLINE)) != 0)
        Rio_writen(conn_fd, buffer, n);

    Close(target_fd);
}

