/*
 * ftp.c - Transfer a file to a connection
 */

#include <sys/stat.h>
#include <string.h>
#include "csapp.h"
#include "utils.h"

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
    send_message(conn_fd, "Success\n");

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

