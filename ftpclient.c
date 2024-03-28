/*
 * ftpclient.c - A FTP client
 */

#include <string.h>
#include <assert.h>
#include <time.h>
#include "csapp.h"

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
    char *host, dest_path[MAXLINE], buffer[MAXLINE];
    long long nb_total, nb_received, nb_remaining;
    clock_t start_transfer, end_transfer;
    rio_t client_rio, dest_rio;
    double transfer_time;
    int n;

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
        // Send the file path
        Rio_writen(client_fd, buffer, strlen(buffer));
        strcpy(dest_path, buffer);

        // Receive error code
        receive_line(&client_rio, buffer, MAXLINE);
        Fputs(buffer, stdout);
        if (strcmp(buffer, "Success\n") != 0)
            continue;
        
        // Generate output destination
        dest_fd = generate_dest_fd(dest_path);
        Rio_readinitb(&dest_rio, dest_fd);

        // Receive the size of the file
        receive_line(&client_rio, buffer, MAXLINE);
        nb_total = atoll(buffer);

        // Loop until received everything
        start_transfer = clock();
        nb_received = 0;
        nb_remaining = nb_total;
        while (nb_received < nb_total) {
            if (nb_remaining < MAXLINE)
                n = receive_nbytes(&client_rio, buffer, nb_remaining);
            else
                n = receive_nbytes(&client_rio, buffer, MAXLINE);
            nb_received += n;
            nb_remaining -= n;
            Rio_writen(dest_fd, buffer, n);
        }
        end_transfer = clock();
    
        // Receive ending and verify properties
        receive_line(&client_rio, buffer, MAXLINE);
        assert(strcmp(buffer, "End\n") == 0);
        assert(nb_received == nb_total);
        
        // Print information on the transfer
        transfer_time = (double)(end_transfer - start_transfer) / CLOCKS_PER_SEC;
        printf("Transfer successfully complete.\n");
        printf("%lld bytes received in %f seconds (%f bytes/s).\n", 
                nb_received, transfer_time, transfer_time / nb_received);
    }

    end_session(client_fd);
}
