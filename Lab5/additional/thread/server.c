#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define MAXSIZE 100

typedef struct {
    char route[50];
    int total_seats;
    int booked_seats;
} Route;

Route routes[2] = {
    {"CityA to CityB", 10, 0},
    {"CityC to CityD", 15, 0}
};

pthread_mutex_t lock;

void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    free(client_socket);
    char buffer[MAXSIZE];
    int requested_seats, route_choice;

    recv(sock, buffer, sizeof(buffer), 0);
    sscanf(buffer, "%d %d", &route_choice, &requested_seats);

    pthread_mutex_lock(&lock);

    if (route_choice < 1 || route_choice > 2) {
        sprintf(buffer, "Invalid route selection.\n");
    } else if (routes[route_choice - 1].booked_seats + requested_seats <= routes[route_choice - 1].total_seats) {
        routes[route_choice - 1].booked_seats += requested_seats;
        sprintf(buffer, "Booking confirmed: %d seats for %s\n", requested_seats, routes[route_choice - 1].route);
    } else {
        sprintf(buffer, "Not enough seats available. Only %d left.\n",
                routes[route_choice - 1].total_seats - routes[route_choice - 1].booked_seats);
    }

    pthread_mutex_unlock(&lock);

    send(sock, buffer, strlen(buffer), 0);
    close(sock);
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t thread_id;

    pthread_mutex_init(&lock, NULL);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    printf("Server started. Waiting for clients...\n");

    while (1) {
        addr_size = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);
        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        int *client_sock = malloc(sizeof(int));
        *client_sock = client_fd;
        pthread_create(&thread_id, NULL, handle_client, client_sock);
        pthread_detach(thread_id);
    }

    pthread_mutex_destroy(&lock);
    close(server_fd);
    return 0;
}
