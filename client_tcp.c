#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include <cjson/cJSON.h> 

#define MAXSIZE 50

int sockfd;

void* receiveMessages(void* arg) {
    char buff[MAXSIZE];
    int recedbytes;

    while (1) {
        memset(buff, '\0', sizeof(buff));
        recedbytes = recv(sockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1) {
            printf("Error receiving data\n");
            break;
        }
        printf("Received from server: %s\n", buff);
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
        printf("Enter the text: ");
        scanf("%s", buff);

        // Send message to server
        sentbytes = send(sockfd, buff, strlen(buff), 0);
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
    struct sockaddr_in serveraddr;
    int retval;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket Creation Error");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("Connection error");
        return -1;
    }

    pthread_t receiveThread, sendThread;

    // Create threads for receiving and sending messages
    pthread_create(&receiveThread, NULL, receiveMessages, NULL);
    pthread_create(&sendThread, NULL, sendMessages, NULL);
    
    
    while (1) {
        printf("\nMenu:\n");
        printf("1. Search for a number\n");
        printf("2. Sort the set in ascending order\n");
        printf("3. Sort the set in descending order\n");
        printf("4. Split the set into odd and even numbers\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 5) {
            send(sockfd, &choice, sizeof(int), 0);
            break;  // Exit the client
        }
        
        
	cJSON *json = cJSON_CreateObject(); 
	
	
	
        printf("Enter the size of the set: ");
        scanf("%d", &size);
	cJSON_AddNumberToObject(json, "size", size); 
	
	
        printf("Enter the elements of the set: ");
        cJSON *int_array = cJSON_CreateArray();
        for (int i = 0; i < size; i++) {
            scanf("%d", &arr[i]);
             cJSON_AddItemToArray(int_array, cJSON_CreateNumber(arr[i));
        }
	
        // Send the size of the array and the elements to the server
        //send(sockfd, &size, sizeof(int), 0);
        //send(sockfd, arr, sizeof(int) * size, 0);

        // Send the choice to the server
        send(sockfd, &choice, sizeof(int), 0);
        cJSON_AddNumberToObject(json, "choice", choice);
	
        if (choice == 1) {
            printf("Enter the number to search: ");
            scanf("%d", &num);
            send(sockfd, &num, sizeof(int), 0);
        }

        // Receive the result from the server
        recv(sockfd, buffer, sizeof(buffer), 0);
        printf("Server Response: %s\n", buffer);
    }
    
    
    // Wait for both threads to complete
    pthread_join(receiveThread, NULL);
    pthread_join(sendThread, NULL);

    close(sockfd);
    return 0;
}

