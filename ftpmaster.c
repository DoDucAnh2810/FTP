#include "csapp.h"
#include "utils.h"
#include "cJSON.h"

static int next_cluster_index = 0;

int select_cluster_index(int nb_cluster) {
    int selected_cluster = next_cluster_index;
    next_cluster_index = (next_cluster_index + 1) % nb_cluster; // Passer au prochain serveur esclave
    return selected_cluster;
}

int main(int argc, char **argv) {
    cluster_t *cluster_list, cluster;
    int listenfd, connfd, nb_cluster;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    char message[MAXLINE];

    cluster_list = parse_cluster_list("clusters.json", &nb_cluster);
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(MASTER_PORT);
    printf("Master is listening on port %d\n", MASTER_PORT);

    while (1) {
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);
        
        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                INET_ADDRSTRLEN);

        cluster = cluster_list[select_cluster_index(nb_cluster)];
        sprintf(message, "%s %s %d\n", cluster.name, cluster.host, cluster.port);
        send_message(connfd, message);
        Close(connfd);

        printf("Assign Cluster %s connection to (%s)\n", cluster.name, client_ip_string);
    }

    exit(0);
}