/*
 * ftpserver.c - A pooled FTP server
 */

#include "csapp.h"
#include "utils.h"
#include "ftp.h"

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

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv) {
    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);
    
    int listenfd, connfd, i;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    pid_t pid;
    rio_t conn_rio;
    int n;
    char buffer[MAXLINE];
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(SERVER_PORT);

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
        
        printf("Server connected to %s (%s)\n", client_hostname,
            client_ip_string);

        // Loop through requests sent by client
        Rio_readinitb(&conn_rio, connfd);
        while ((n = Rio_readlineb(&conn_rio, buffer, MAXLINE)) != 0) {
            if (n <= 1) {
                send_message(connfd, "Empty request\n");
                continue;
            }
            buffer[n-1] = '\0'; // get rid of \n at the end

            printf("Received request for %s from %s (%s)\n", 
                    buffer, client_hostname, client_ip_string);
                    
            ftp(connfd, buffer);

            printf("Successfully completed request for %s from %s (%s)\n", 
                                buffer, client_hostname, client_ip_string);
        }

        Close(connfd);    
    }

    exit(0);
}

