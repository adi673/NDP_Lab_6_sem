#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/file.h>

#define PORT 12345
#define MAXSIZE 100
#define SEAT_FILE "seats.txt"  // File to track available seats

typedef struct {
    char route[50];
    int total_seats;
    int booked_seats;
} Route;

Route routes[2] = {
    {"CityA to CityB", 10, 0},
    {"CityC to CityD", 15, 0}
};

// Function to load seat data from the file
void load_seats_from_file() {
    FILE *file = fopen(SEAT_FILE, "r");
    if (file != NULL) {
        fscanf(file, "%d %d", &routes[0].booked_seats, &routes[1].booked_seats);
        fclose(file);
    }
}

// Function to save seat data to the file
void save_seats_to_file() {
    FILE *file = fopen(SEAT_FILE, "w");
    if (file != NULL) {
        fprintf(file, "%d %d\n", routes[0].booked_seats, routes[1].booked_seats);
        fclose(file);
    }
}

void handle_client(int client_sock) {
    char buffer[MAXSIZE];
    int requested_seats, route_choice;

    // Receive request from client
    recv(client_sock, buffer, sizeof(buffer), 0);
    sscanf(buffer, "%d %d", &route_choice, &requested_seats);

    if (route_choice < 1 || route_choice > 2) {
        sprintf(buffer, "Invalid route selection.\n");
    } else {
        // Load current seat data from file
        load_seats_from_file();

        // Lock the file to prevent race conditions
        int fd = open(SEAT_FILE, O_RDWR);
        if (fd == -1) {
            perror("Error opening seat file");
            exit(1);
        }

        if (flock(fd, LOCK_EX) == -1) {
            perror("Error locking file");
            exit(1);
        }

        // Check if enough seats are available
        if (routes[route_choice - 1].booked_seats + requested_seats <= routes[route_choice - 1].total_seats) {
            routes[route_choice - 1].booked_seats += requested_seats;
            sprintf(buffer, "Booking confirmed: %d seats for %s\n", requested_seats, routes[route_choice - 1].route);
        } else {
            sprintf(buffer, "Not enough seats available. Only %d left.\n",
                    routes[route_choice - 1].total_seats - routes[route_choice - 1].booked_seats);
        }

        // Save the updated seat data back to file
        save_seats_to_file();

        // Unlock the file
        flock(fd, LOCK_UN);
        close(fd);
    }

    // Send result back to client
    send(client_sock, buffer, strlen(buffer), 0);
    close(client_sock);
    exit(0);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // Initialize the seat file
    FILE *file = fopen(SEAT_FILE, "w");
    if (file == NULL) {
        perror("Error initializing seat file");
        exit(1);
    }
    fprintf(file, "0 0\n");  // Initially no seats booked
    fclose(file);

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

        pid_t pid = fork();
        if (pid == 0) {  // Child process handles client
            close(server_fd);
            handle_client(client_fd);
        } else {  // Parent process
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}
