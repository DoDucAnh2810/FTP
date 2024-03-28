/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include "string.h"

#define PORT 2121

static int client_fd = -1;
static int dest_fd = -1;

void end_session() {
    if (client_fd != -1)
        Close(client_fd);
    if (dest_fd != -1)
        Close(dest_fd);
    exit(0);
}

int receive_line(rio_t *client_rp, char *buffer, size_t maxlen) {
    int n = Rio_readlineb(client_rp, buffer, maxlen); 
    if (n <= 0)
        end_session();
    return n;   
}

int receive_nbytes(rio_t *client_rp, char *buffer, size_t maxlen) {
    int n = Rio_readnb(client_rp, buffer, maxlen); 
    if (n <= 0)
        end_session();
    return n;   
}

int generate_dest_fd(char *dest_path) {
    int i, suffix_index, dest_fd;
    char suffix_string[8];

    i = 0;
    while (dest_path[i] != '\n')
        i++;
    dest_path[i] = '\0';

    suffix_index = 0;
    dest_fd = open(dest_path, O_WRONLY | O_CREAT, 0644);
    while (dest_fd == -1) {
        sprintf(suffix_string, "-%d", suffix_index);
        dest_path[i] = '\0';
        strcat(dest_path, suffix_string);
        dest_fd = open(dest_path, O_WRONLY | O_CREAT, 0644);
    }

    return dest_fd;
}

int main(int argc, char **argv)
{
    int n;
    char *host, dest_path[MAXLINE], buffer[MAXLINE];
    long long nb_total, nb_received;
    rio_t client_rio, dest_rio;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    client_fd = Open_clientfd(host, PORT);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n"); 
    
    Rio_readinitb(&client_rio, client_fd);

    while (Fgets(buffer, MAXLINE, stdin) != NULL) {
        Rio_writen(client_fd, buffer, strlen(buffer));
        strcpy(dest_path, buffer);

        receive_line(&client_rio, buffer, MAXLINE);
        Fputs(buffer, stdout);

        if (strcmp(buffer, "Success\n") != 0)
            continue;
        
        dest_fd = generate_dest_fd(dest_path);
        Rio_readinitb(&dest_rio, dest_fd);

        receive_line(&client_rio, buffer, MAXLINE);
        nb_total = atoll(buffer);
        nb_received = 0;

        while (nb_received < nb_total) {
            n = receive_nbytes(&client_rio, buffer, MAXLINE);
            nb_received += n;
            Rio_writen(dest_fd, buffer, n);
        }
    }
    end_session(client_fd);
}
