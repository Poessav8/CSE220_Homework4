#include "hw4.h"

int main() {
    printf("SERVER!!!\n");
    int listenfd, connfd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    ChessGame game;

    // Create socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set options to reuse the IP address and IP port if either is already in use
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))");
        return -1;
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))");
        return -1;
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(listenfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(listenfd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    INFO("Server listening on port %d", PORT);
    // Accept incoming connection
    if ((connfd = accept(listenfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    initialize_game(&game);
    display_chessboard(&game);


    while (1) {
        char client_input[BUFFER_SIZE];
        read(connfd, client_input, BUFFER_SIZE);
        int client_command_result = receive_command(&game, client_input, connfd, false);
        printf("CLIENT COMMAND RESULT: %d\n", client_command_result);
        if (client_command_result == COMMAND_FORFEIT) {
            close(connfd);
            break;
        }
        // Server player prompt for input
        char server_input[BUFFER_SIZE];
        printf("[Server] Enter Command: ");
        fgets(server_input, BUFFER_SIZE, stdin);
        server_input[strcspn(server_input, "\n")] = 0; // remove newline character
        int server_command_result = send_command(&game, server_input, connfd, false);
        printf("SERVER COMMAND RESULT: %d\n", server_command_result);
        while (server_command_result == COMMAND_ERROR || server_command_result == COMMAND_UNKNOWN) {
            printf("[Server] Invalid command. Enter Command: ");
            fgets(server_input, BUFFER_SIZE, stdin);
            server_input[strcspn(server_input, "\n")] = 0; // remove newline character
            server_command_result = send_command(&game, server_input, connfd, false);
        }
        if (server_command_result == COMMAND_FORFEIT) {
            printf("UH OH!!!!\n");
            close(connfd);
            break;
        }
    }
    FILE *temp = fopen("./fen.txt", "w");
    char fen[200];
    chessboard_to_fen(fen, &game);
    fprintf(temp, "%s", fen);
    fclose(temp);
    close(listenfd);
    return 0;
}
