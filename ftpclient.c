/*
 * ftpclient.c - A FTP client
 */

#include <string.h>
#include <assert.h>
#include <time.h>
#include "csapp.h"
#include "utils.h"

/* Generate a good file descriptor for writing to dest_path*/
int generate_dest_fd(char *dest_path) {
    int i, suffix_index, dest_fd;
    char suffix_string[8];

    // Find the end of the string
    i = 0;
    while (dest_path[i] != '\n')
        i++;
    dest_path[i] = '\0';

    // Loop until a valid path
    suffix_index = 1;
    dest_fd = open(dest_path, O_WRONLY | O_CREAT, 0644);
    while (dest_fd == -1) {
        sprintf(suffix_string, "-%d", suffix_index);
        dest_path[i] = '\0';
        strcat(dest_path, suffix_string);
        dest_fd = open(dest_path, O_WRONLY | O_CREAT, 0644);
    }

    return dest_fd;
}

/* Get command line of user from stdin */
char *get_command(char *buffer) {
    printf("\x1b[1;33mftp>\x1b[0m ");
    return Fgets(buffer, MAXLINE, stdin);
}

int main(int argc, char **argv) {
    char *host, dest_path[MAXLINE], buffer[MAXLINE];
    long long nb_total, nb_received, nb_remaining;
    clock_t start_transfer, end_transfer;
    rio_t client_rio, dest_rio;
    int client_fd, dest_fd;
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
    client_fd = Open_clientfd(host, SERVER_PORT);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("Connected to FTP server\n"); 
    
    
    Rio_readinitb(&client_rio, client_fd);
    while (get_command(buffer) != NULL) {
        // Send the file path
        send_message(client_fd, buffer);
        strcpy(dest_path, buffer);

        // Receive error code
        Rio_readlineb(&client_rio, buffer, MAXLINE);
        if (are_equal_strings(buffer, "Successful request\n"))
            printf("\x1b[1;32m%s\x1b[0m", buffer);
        else {
            printf("\x1b[1;31m%s\x1b[0m", buffer);
            continue;
        }

        // Generate output destination
        dest_fd = generate_dest_fd(dest_path);
        Rio_readinitb(&dest_rio, dest_fd);

        // Receive the size of the file
        Rio_readlineb(&client_rio, buffer, MAXLINE);
        nb_total = atoll(buffer);

        // Loop until received everything
        start_transfer = clock();
        nb_received = 0;
        nb_remaining = nb_total;
        while (nb_received < nb_total) {
            if (nb_remaining < MAXLINE)
                n = Rio_readnb(&client_rio, buffer, nb_remaining);
            else
                n = Rio_readnb(&client_rio, buffer, MAXLINE);
            nb_received += n;
            nb_remaining -= n;
            Rio_writen(dest_fd, buffer, n);
        }
        end_transfer = clock();
    
        // Receive ending and verify properties
        Rio_readlineb(&client_rio, buffer, MAXLINE);
        assert(are_equal_strings(buffer, "End\n"));
        assert(nb_received == nb_total);
        
        // Print information on the transfer
        transfer_time = (double)(end_transfer - start_transfer) / CLOCKS_PER_SEC;
        printf("Transfer successfully completed.\n");
        printf("%lld bytes received in %lf seconds (%.2f KBs/s).\n", 
                nb_received, transfer_time, (nb_received / transfer_time) / 1000);
    }

    Close(client_fd);
    Close(dest_fd);
    exit(0);
}
