#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "hw4.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    ChessGame game;
    int connfd = 0;
    struct sockaddr_in serv_addr;

    // Connect to the server
    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(connfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    initialize_game(&game);
    display_chessboard(&game);

    while (1) {
        // Fill this in
        char input[BUFFER_SIZE];
        printf("[Client] Enter Command: ");
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = 0; // remove newline character
        int send_result = send_command(&game, input, connfd, true);
    while (send_result == COMMAND_ERROR || send_result == COMMAND_UNKNOWN || send_result == COMMAND_SAVE) {
        printf("[Client] Invalid command. Enter another command: ");
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = 0; // remove newline character
        send_result = send_command(&game, input, connfd, true);
    
    }
    printf("SEND RESULT: %d\n", send_result);
    if (send_result == COMMAND_FORFEIT) {
        close(connfd);
        break;
    }
    char server_input[BUFFER_SIZE];
    read(connfd, server_input, BUFFER_SIZE);
    int receive_result = receive_command(&game, server_input, connfd, true);
    printf("RECIEVE RESULT_client: %d\n", receive_result);
    if (receive_result == COMMAND_FORFEIT) {
        close(connfd);
        break;
    }
    }
    // Please ensure that the following lines of code execute just before your program terminates.
    // If necessary, copy and paste it to other parts of your code where you terminate your program.
    FILE *temp = fopen("./fen.txt", "w");
    char fen[200];
    chessboard_to_fen(fen, &game);
    fprintf(temp, "%s", fen);
    fclose(temp);
    close(connfd);
    return 0;
}
