#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<pthread.h>

#define MAXSIZE 90

int sockfd, newsockfd;

void* receiveMessages(void* arg) {
    char buff[MAXSIZE];
    int recedbytes;

    while (1) {
        memset(buff, '\0', sizeof(buff));
        recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1) {
            printf("Error receiving data\n");
            break;
        }
        printf("Received from client: %s\n", buff);
        // Check for "stop" message
        if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p') {
            break;
        }
    }
    return NULL;
}

void* sendMessages(void* arg) {
    char buff[MAXSIZE];
    int sentbytes;

    while (1) {
        memset(buff, '\0', sizeof(buff));
        printf("Enter server reply: ");
        scanf("%s", buff);

        // Send message to client
        sentbytes = send(newsockfd, buff, strlen(buff), 0);
        if (sentbytes == -1) {
            printf("Error sending data\n");
            break;
        }

        // Check for "stop" message to end communication
        if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p') {
            break;
        }
    }
    return NULL;
}

int main() {
    int retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket creation error");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("Binding error");
        close(sockfd);
        return -1;
    }

    retval = listen(sockfd, 1);
    if (retval == -1) {
        close(sockfd);
        return -1;
    }

    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
    if (newsockfd == -1) {
        close(sockfd);
        return -1;
    }

    pthread_t receiveThread, sendThread;

    // Create threads for receiving and sending messages
    pthread_create(&receiveThread, NULL, receiveMessages, NULL);
    pthread_create(&sendThread, NULL, sendMessages, NULL);

    // Wait for both threads to complete
    pthread_join(receiveThread, NULL);
    pthread_join(sendThread, NULL);

    close(newsockfd);
    close(sockfd);
    return 0;
}

