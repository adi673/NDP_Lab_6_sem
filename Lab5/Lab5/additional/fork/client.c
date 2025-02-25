#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAXSIZE 100

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[MAXSIZE];
    int route_choice, num_seats;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Available Routes:\n");
    printf("1. CityA to CityB (10 seats)\n");
    printf("2. CityC to CityD (15 seats)\n");

    printf("Enter route number (1 or 2): ");
    scanf("%d", &route_choice);
    printf("Enter number of seats to book: ");
    scanf("%d", &num_seats);

    sprintf(buffer, "%d %d", route_choice, num_seats);
    send(sock, buffer, strlen(buffer), 0);

    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server Response: %s\n", buffer);

    close(sock);
    return 0;
}
