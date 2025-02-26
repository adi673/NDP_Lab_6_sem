#define MAXSIZE 100
// String
int main()
{
    char buff[MAXSIZE];
    memset(buff, 0, sizeof(buff));
    buff[strcspn(buff, "\n")] = '\0';
    if (strcmp(buff, "stop") == 0)
    {
        // break;
    }
    fgets(buff, MAXSIZE, stdin);
    int sentbytes = send(sockfd, buff, strlen(buff), 0);

    memset(buff, 0, sizeof(buff));
    int recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
}

// Integer

int main()
{
    int num = 42;                       // Integer to send
    send(sockfd, &num, sizeof(num), 0); // Send integer directly
    int num;
    recv(newsockfd, &num, sizeof(num), 0); // Receive integer directly
    printf("Received: %d\n", num);

    // Array of integers
    int numbers[] = {10, 20, 30, 40, 50};
    send(sockfd, numbers, sizeof(numbers), 0);
    int numbers[5];
    recv(newsockfd, numbers, sizeof(numbers), 0);
    printf("Received numbers: %d, %d, %d, %d, %d\n", numbers[0], numbers[1], numbers[2], numbers[3], numbers[4]);

    // passig interas string
    char buffer[10];
    sprintf(buffer, "%d", 42); // Convert integer to string
    send(sockfd, buffer, strlen(buffer), 0);
    char buffer[10];
    recv(newsockfd, buffer, sizeof(buffer), 0);
    int num = atoi(buffer); // Convert string to integer

    // alphanumeric String
    char buffer[100];
    strcpy(buffer, "User1234Code567");
    send(sockfd, buffer, strlen(buffer), 0);
    char buffer[100];
    recv(newsockfd, buffer, sizeof(buffer), 0);
    printf("Received: %s\n", buffer);
    int extractedNumber;
    char letters[50];
    extractData(buffer, letters, &extractedNumber);

    printf("Letters: %s\n", letters);        // Output: UserCode
    printf("Number: %d\n", extractedNumber); // Output: 1234567
    void extractData(const char *str, char *letters, int *number)
    {
        int i, letterIndex = 0;
        char numStr[50]; // Temporary string to hold extracted digits
        int numberIndex = 0;

        for (i = 0; str[i] != '\0'; i++)
        {
            if (isalpha(str[i]))
            {
                letters[letterIndex++] = str[i]; // Store letters
            }
            else if (isdigit(str[i]))
            {
                numStr[numberIndex++] = str[i]; // Store numbers as a string
            }
        }

        letters[letterIndex] = '\0'; // Null terminate letters string
        numStr[numberIndex] = '\0';  // Null terminate number string

        *number = (numberIndex > 0) ? atoi(numStr) : 0; // Convert to integer
    }
}

// matrix Passing

int main()
{
    // client
    int rows = 2;
    int cols = 3;
    send(sockfd, &rows, sizeof(rows), 0);
    send(sockfd, &cols, sizeof(cols), 0);
    int matrix[rows][cols] = {{1, 2, 3}, {4, 5, 6}};
    send(sockfd, matrix, sizeof(matrix), 0);

    // server
    int rowserver, colserver;
    recv(newsockfd, &rowserver, sizeof(rowserver), 0);
    recv(newsockfd, &colserver, sizeof(colserver), 0);

    int matrix[rowserver][colserver];
    recv(newsockfd, matrix, sizeof(matrix), 0); // ✅ Use newsockfd here

    // or server
    int **matrix = (int **)malloc(rowserver * sizeof(int *)); // Allocate row pointers
    for (int i = 0; i < rows; i++)
    {
        matrix[i] = (int *)malloc(colserver * sizeof(int)); // Allocate columns
    }

    // Receive entire matrix
    recv(newsockfd, matrix[0], rowserver * colserver * sizeof(int), 0);
}


//Structure

int main(){
    //client
    struct Student {
        int id;
        char name[50];
        float marks;
    };
    struct Student s1 = {101, "Alice", 90.5};
    send(newsockfd, &s1, sizeof(s1), 0);  // Send structure

    //server
    struct Student {
        int id;
        char name[50];
        float marks;
    };
    struct Student s1;
    recv(sockfd, &s1, sizeof(s1), 0);  // Receive structure
    printf("Received -> ID: %d, Name: %s, Marks: %.2f\n", s1.id, s1.name, s1.marks);

}