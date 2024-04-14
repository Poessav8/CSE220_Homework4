#include "hw4.h"

//part 1
void initialize_game(ChessGame *game) {
    (void)game;

    game->moveCount = 0;
    game->capturedCount = 0;
    game->currentPlayer = WHITE_PLAYER;

    char blackPieces[8] = {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'};
    for (int i = 0; i < 8; i++) {
        game->chessboard[0][i] = blackPieces[i];
    }
    for(int i = 0; i<8; i++){
        game->chessboard[1][i] = 'p';
    }
    for(int i = 2; i<6; i++){
        for(int j = 0; j<8; j++){
            game->chessboard[i][j] = '.';
        }
    }
    for(int i = 0; i<8; i++){
        game->chessboard[6][i] = 'P';
    }
    char whitePieces[8] = {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};
    for (int i = 0; i < 8; i++) {
        game->chessboard[7][i] = whitePieces[i];
    }

}

void chessboard_to_fen(char fen[], ChessGame *game) {
    int index = 0; // Index to keep track of position in the FEN string
    char curr_player = game->currentPlayer;

    for (int i = 0; i < 8; i++) {
        int count = 0; // Counter for consecutive empty squares
        for (int j = 0; j < 8; j++) {
            if (game->chessboard[i][j] == '.') {
                count++;
            } else {
                if (count > 0) {
                    fen[index++] = count + '0'; // Convert count to character and add to FEN string
                    count = 0; // Reset count
                }
                fen[index++] = game->chessboard[i][j]; // Add piece to FEN string
            }
        }
        if (count > 0) {
            fen[index++] = count + '0'; // Add remaining count to FEN string
        }
        if (i < 8 - 1) {
            fen[index++] = '/'; // Add '/' between rows except for the last row
        }
    }

    fen[index++] = ' '; // Add space after board
    fen[index++] = (curr_player == WHITE_PLAYER) ? 'w' : 'b'; // Add current player
    fen[index] = '\0'; // Null-terminate the string
}


bool is_valid_pawn_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {

    //if rows moved is greater than two
    if(abs(src_row - dest_row) > 2){
	    return false;
	  }    
    //if black:
    if(!isupper(piece)){
	    //if there is an opposing piece on the destination row and column
	    if(isupper(game->chessboard[dest_row][dest_col])){
		    //if player is moving diagonal
		    if(abs(src_col - dest_col) == 1){
			    //can only move down by one, can't move purely sideways
			    return (dest_row - src_row == 1 )? true: false;
			} else {
			   return false; //cant capture by moving forwards
			}
		} else{ //if there is not an opposing piece on the dest row and column
			//if player is moving diagonally by any amount:
			if(abs(src_col - dest_col) > 0 ){
				return false;
			}
            //if there is already a piece (not enemy) on destination:
            if(game->chessboard[dest_row][dest_col] != '.'){
                return false;
            }
			//else they must be moving forward. 
			if(dest_row - src_row == 2){//if they move forward two
                if(game->chessboard[src_row+1][dest_col] != '.'){ //can't have any obstructions to path
                    return false;
                }
				return (src_row == 1) ? true: false;
			}else{
				return (dest_row - src_row == 1)? true: false; //can't not move
			}
        }    
    } else{ //is white -- DOESN'T SEEM TO WORK>???
	//if there is an opposing piece on the destination row and column
	    if(!isupper(game->chessboard[dest_row][dest_col]) && game->chessboard[dest_row][dest_col] != '.'){
		    //if player is moving diagonal
		    if(abs(src_col - dest_col) == 1){
			    //can only move up by one, can't move purely sideways
			    return (src_row - dest_row == 1 )? true: false;
			} else{
				return false;
			}
		} else{ //if there is not an opposing piece on the dest row and column
			//if player is moving diagonally by any amount:
			if(abs(src_col - dest_col) > 0 ){
				return false;
			}
            //if there is already a piece (not enemy) on destination:
            if(isupper(game->chessboard[dest_row][dest_col])){
                return false;
            }
			//else they must be moving forward (upwards). 
			if(src_row - dest_row == 2){//if they move forward two
            if(game->chessboard[src_row-1][dest_col] != '.'){ //can't have any obstructions to path
                return false;
            }
            return (src_row == 6) ? true: false; //they mustve started from the correct row
			}else{
				return (src_row - dest_row == 1)? true: false; //they have to move
			}  
	    }
    }
    
}


//temp func
bool is_valid_rook_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    if (src_row != dest_row && src_col != dest_col) {
        return false;
    }
    int start, end;
    if (src_row == dest_row) { 
        start = (src_col > dest_col) ? dest_col : src_col;
        end = (src_col > dest_col) ? src_col : dest_col;
        for (int i = start + 1; i < end; i++) {
            if (game->chessboard[src_row][i] != '.') {
                return false;
            }
        }
    } else if (src_col == dest_col) { 
        start = (src_row > dest_row) ? dest_row : src_row;
        end = (src_row > dest_row) ? src_row : dest_row;
        for (int i = start + 1; i < end; i++) {
            if (game->chessboard[i][src_col] != '.') {
                return false;
            }
        }
    } else { 
        return false;
    }
    return true;
}



bool is_valid_knight_move(int src_row, int src_col, int dest_row, int dest_col) {
    if(src_row == dest_row || src_col == dest_col){ //if only moves horizontal/vertical or both
	    return false;
    }
	if(abs(src_row - dest_row) == 1){ //moved horizontally then up/down one
		//check that moved horizontally by two
		return (abs(src_col - dest_col) == 2) ? true: false;
	} else if (abs(src_col - dest_col) == 1){
		return (abs(src_row - dest_row) == 2) ? true: false;
	} else{
		return false;
	}
	
}



//temp func
bool is_valid_bishop_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    if (abs(src_row - dest_row) != abs(src_col - dest_col)) {
        return false;
    }
    if (dest_col > src_col) { 
        if (dest_row > src_row) { 
            int i = src_row + 1;
            int j = src_col + 1;
            while (i < dest_row) {
                if (game->chessboard[i][j] != '.') {
                    return false;
                }
                i++, j++;
            }
        } else { 
            int i = src_row - 1;
            int j = src_col + 1;
            while (i > dest_row) {
                if (game->chessboard[i][j] != '.') {
                    return false;
                }
                i--, j++;
            }
        }
    } else { 
        if (dest_row > src_row) { 
            int i = src_row + 1;
            int j = src_col - 1;
            while (i < dest_row) {
                if (game->chessboard[i][j] != '.') {
                    return false;
                }
                i++, j--;
            }
        } else { 
            int i = src_row - 1;
            int j = src_col - 1;
            while (i > dest_row) {
                if (game->chessboard[i][j] != '.') {
                    return false;
                }
                i--, j--;
            }
        }
    }
    return true;
}



//FIX QUEEN!!!!
bool is_valid_queen_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	if((src_row != dest_row) && (src_col != dest_col)){ //bishop
		return is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game);
	} else{
		return is_valid_rook_move(src_row, src_col, dest_row, dest_col, game);
	}
}


bool is_valid_king_move(int src_row, int src_col, int dest_row, int dest_col) {
    //Verifies that the king is moving one square in any direction (horizontally, vertically, or diagonally).
    if(abs(src_col - dest_col)>1 || abs(dest_row - src_row) > 1){
        return false;
    }
    if(src_col == dest_col && src_row == dest_row){
        return false;
    }
    return true;

}

bool is_valid_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    
    if(game->chessboard[src_row][src_col] == '.'){
        return false;
    }
    if(piece == 'P' || piece == 'p'){
        return is_valid_pawn_move(piece, src_row, src_col, dest_row, dest_col, game);
    }
    if(piece == 'R' || piece == 'r' ){
        return is_valid_rook_move(src_row, src_col, dest_row, dest_col, game);
    }
    if(piece == 'N' || piece == 'n'){
        return is_valid_knight_move(src_row, src_col, dest_row, dest_col);
    }
    if(piece == 'B' || piece == 'b'){
        return is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game);
    }
    if(piece == 'Q' || piece == 'q'){
        return is_valid_queen_move(src_row, src_col, dest_row, dest_col, game);
    }
    if(piece == 'K' || piece == 'k'){
        return is_valid_king_move(src_row, src_col, dest_row, dest_col);
    }
    return false;
}

void fen_to_chessboard(const char *fen, ChessGame *game) {
    int row = 0, col = 0;
    int i = 0;
    printf("ENTERED FEN_TO_CHESS\n");
    printf("fen string: %s\n", fen);
    while (fen[i] != ' ') {
        printf("FEN LOOP 1\n");
        if (fen[i] >= '1' && fen[i] <= '8') {
            int emptySquares = fen[i] - '0';
            for (int j = 0; j < emptySquares; j++) {
                game->chessboard[row][col++] = '.';
            }
        } else if (fen[i] == '/') {
            row++;
            col = 0;
        } else {
            game->chessboard[row][col++] = fen[i];
        }
        i++;
    }
    i++; // skip space
    game->currentPlayer = fen[i] == 'w' ? WHITE_PLAYER : BLACK_PLAYER;
}


int parse_move(const char *move, ChessMove *parsed_move) {
    (void)move;
    (void)parsed_move;
    if (strlen(move) != 4 && strlen(move) != 5)
        return PARSE_MOVE_INVALID_FORMAT; // Return error code for invalid format

    char from_col = move[0];
    char from_row = move[1];
    char to_col = move[2];
    char to_row = move[3];

    // Check if the row letter is in the range 'a' through 'h'
    if (from_col < 'a' || from_col > 'h' || to_col < 'a' || to_col > 'h')
        return PARSE_MOVE_INVALID_FORMAT; // Return error code for invalid format

    if(from_row < '1' || from_row > '8' || to_row < '1' || to_row > '8'){
        return PARSE_MOVE_OUT_OF_BOUNDS;
    }

    if(strlen(move) == 5){
        char promotion = move[4];
        if(to_row != '1' && to_row != '8'){
            return PARSE_MOVE_INVALID_DESTINATION;
        }

        if(promotion != 'q' && promotion != 'r' && promotion != 'b' && promotion != 'n'){
            return PARSE_MOVE_INVALID_PROMOTION;
        }
    }
    strncpy(parsed_move->startSquare, move, 2);
    parsed_move->startSquare[2] = '\0'; // Null-terminate the string
    if(strlen(move) == 4){
     strncpy(parsed_move->endSquare, move + 2, 2);
    parsed_move->endSquare[2] = '\0'; // Null-terminate the string   
    }else{
        strncpy(parsed_move->endSquare, move + 2, 3);
    parsed_move->endSquare[3] = '\0'; // Null-terminate the string  
    }
    

    return 0;
}


int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move) {
    // any required variable declarations go here
    int start_row = move->startSquare[1] - '1';
    start_row = 7 - start_row;
    int start_col = move->startSquare[0] - 'a';
    int end_row =  move->endSquare[1] - '1';
    end_row = 7 - end_row;
    int end_col = move->endSquare[0] - 'a';

    if (validate_move) {
        // error checks go here

        printf("START: game->chessboard[%d][%d], END: game->chessboard[%d][%d]\n", start_row, start_col, end_row, end_col);

        // MOVE_OUT_OF_TURN
        if ((is_client && (game->currentPlayer != WHITE_PLAYER)) || (!is_client && (game->currentPlayer != BLACK_PLAYER))) {
            return MOVE_OUT_OF_TURN;
        }

        // MOVE_NOTHING
        if (game->chessboard[start_row][start_col] == '.') {
            return MOVE_NOTHING;
        }

        // MOVE_WRONG_COLOR
        if ((is_client && islower(game->chessboard[start_row][start_col])) || (!is_client && isupper(game->chessboard[start_row][start_col]))) {
            return MOVE_WRONG_COLOR; // Return error code for wrong color
        }

        // MOVE_SUS
        if ((!is_client && islower(game->chessboard[end_row][end_col])) || (is_client && isupper(game->chessboard[end_row][end_col]))) {
            return MOVE_SUS;
        }

        // MOVE_NOT_A_PAWN
        if (strlen(move->endSquare) == 3 && (!((game->chessboard[start_row][start_col] == 'p') || (game->chessboard[start_row][start_col] == 'P')))) {
            return MOVE_NOT_A_PAWN;
        }

        // MOVE_MISSING_PROMOTION
        if (strlen(move->endSquare) == 2 && ((game->chessboard[start_row][start_col] == 'p' && end_row == 7) || (game->chessboard[start_row][start_col] == 'P' && end_row == 0))) {
            return MOVE_MISSING_PROMOTION;
        }

        // MOVE_WRONG
        if (!is_valid_move(game->chessboard[start_row][start_col], start_row, start_col, end_row, end_col, game)) {
            return MOVE_WRONG;
        }
    }

    // Update the state of the ChessGame struct
    if (game->chessboard[end_row][end_col] != '.') {
        if (game->capturedCount < MAX_CAPTURED_PIECES) {
            game->capturedPieces[game->capturedCount] = game->chessboard[end_row][end_col];
            game->capturedCount++;
        }
    }
    
    // Update moves
    game->moves[game->moveCount] = *move;
    game->moveCount++;

    // Move the piece
    game->chessboard[end_row][end_col] = game->chessboard[start_row][start_col];
    game->chessboard[start_row][start_col] = '.';

    // Update currentPlayer
    game->currentPlayer = (game->currentPlayer == WHITE_PLAYER) ? BLACK_PLAYER : WHITE_PLAYER;



    // Check for pawn promotion
    if(strlen(move->endSquare) == 3){
        game->chessboard[end_row][end_col] = is_client? toupper(move->endSquare[2]) : move->endSquare[2];    
    }

    return 0;
}



int send_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    printf("MESSAGE: %s\n", message);
    if (strncmp(message, "/move", 5) == 0) {
        ChessMove parsed_move;
        int parse_result = parse_move(message + 6, &parsed_move);
        if (parse_result != 0) {
            printf("command error _ parse failed???\n");
            return COMMAND_ERROR; // Return error if move parsing failed
        }

        int make_move_result = make_move(game, &parsed_move, is_client, true);
        if (make_move_result == 0) {
            send(socketfd, message, strlen(message), 0);
            return COMMAND_MOVE;
        } else {
            printf("command error???make_move_result = %d\n", make_move_result);//move out of turn error
            return COMMAND_ERROR;
        }
    }
    else if (strcmp(message, "/forfeit") == 0) {
        send(socketfd, message, strlen(message), 0);
        return COMMAND_FORFEIT;
    }
    else if (strcmp(message, "/chessboard") == 0) {
        display_chessboard(game);
        return COMMAND_DISPLAY;
    }
    else if (strncmp(message, "/import", 7) == 0 && !is_client) {
        fen_to_chessboard(message + 8, game);
        send(socketfd, message, strlen(message), 0);
        return COMMAND_IMPORT;
    }
    else if (strncmp(message, "/load", 5) == 0) {
        char username[255];
        int game_number;
        if (sscanf(message + 6, "%s %d", username, &game_number) == 2) {
            int load_game_result = load_game(game, username, "game_database.txt", game_number);
            if (load_game_result == 0) {
                send(socketfd, message, strlen(message), 0);
                return COMMAND_LOAD;
            } else {
                return COMMAND_ERROR;
            }
        } else {
            return COMMAND_ERROR;
        }
    }
    else if (strncmp(message, "/save", 5) == 0) {
        char username[255];
        if (sscanf(message + 6, "%s", username) == 1) {
            int save_game_result = save_game(game, username, "game_database.txt");
            if (save_game_result == 0) {
                send(socketfd, message, strlen(message), 0);
                return COMMAND_SAVE;
            } else {
                return COMMAND_ERROR;
            }
        } else {
            return COMMAND_ERROR;
        }
    }
    return COMMAND_UNKNOWN;
    
}

int receive_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    printf("MESSAGE: %s\n", message);
    if (strncmp(message, "/move", 5) == 0) {
        ChessMove parsed_move;
        int parse_result = parse_move(message + 6, &parsed_move);
        if (parse_result != 0) {
            return COMMAND_ERROR; // Return error if move parsing failed
        }
        int make_move_result = make_move(game, &parsed_move, is_client, false);
        if (make_move_result == 0) {
            return COMMAND_MOVE;
        } else {
            printf("COMMAND ERROR!!!\n");
            return COMMAND_ERROR;
        }
    }
    else if (strncmp(message, "/forfeit", 8) == 0) {
        close(socketfd);
        return COMMAND_FORFEIT;
    }
    else if (strncmp(message, "/import", 7) == 0 && is_client) {
        fen_to_chessboard(message + 8, game);
        return COMMAND_IMPORT;
    }
    else if (strncmp(message, "/load", 5) == 0) {
        char username[255];
        int game_number;
        if (sscanf(message + 6, "%s %d", username, &game_number) == 2) {
            int load_game_result = load_game(game, username, "game_database.txt", game_number);
            if (load_game_result == 0) {
                return COMMAND_LOAD;
            } else {
                return COMMAND_ERROR;
            }
        } else {
            return COMMAND_ERROR;
        }
    }
    
    return -1; // Return -1 if no command matched
}

int save_game(ChessGame *game, const char *username, const char *db_filename) {

   // printf("original chessboard:\n");
    display_chessboard(game);
    // Check if the username is empty
    if (*username == '\0') {
        return -1; // Username is empty
    }

    // Check if the username contains spaces
    const char *ptr = username;
    while (*ptr != '\0') {
        if (isspace(*ptr)) {
            return -1; // Username contains a space
        }
        ptr++;
    }


    FILE *file = fopen(db_filename, "a"); // Open the file in append mode
    if (file == NULL) {
        return -1; // Error opening the file
    }
    char fen[255];
    chessboard_to_fen(fen, game); // Generate the FEN string of the current game state
   // printf("GENERATED FEN: %s\n", fen);
    // Write the username and FEN string to the file
    fprintf(file, "%s:%s\n", username, fen);
    fclose(file);
    return 0; // Successful save operation
    
}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {
     FILE *file = fopen(db_filename, "r");
    if (file == NULL) {
        return -1; // Error opening the file
    }
    char *line = NULL;
    size_t line_size = 0;
    ssize_t read;
    int found_game = 0;
    char *fen = NULL;
    while ((read = getline(&line, &line_size, file)) != -1) {
        char *line_copy = strdup(line);
        char *token = strtok(line_copy, ":");
        if (strcmp(token, username) == 0) {
            found_game++;
            if (found_game == save_number) {
                char *fen_start = strtok(NULL, ":");
                if (fen_start != NULL) {
                    // Trim leading and trailing whitespace
                    while (isspace(*fen_start)) fen_start++;
                    char *end = fen_start + strlen(fen_start) - 1;
                    while (end > fen_start && isspace(*end)) end--;
                    *(end + 1) = '\0';
                    fen = fen_start;
                }
                printf("READ FEN STRING: %s\n", fen);
                if (fen != NULL) {
                    initialize_game(game);
                    fen_to_chessboard(fen, game);
                    fclose(file);
                    free(line);
                    free(line_copy);
                    return 0; // Successfully loaded the game
                }
            }
        }
        free(line_copy);
    }
    fclose(file);
    free(line);
    return -1; // Game state not found for the given username and save number
}

void display_chessboard(ChessGame *game) {
    printf("\nChessboard:\n");
    printf("  a b c d e f g h\n");
    for (int i = 0; i < 8; i++) {
        printf("%d ", 8 - i);
        for (int j = 0; j < 8; j++) {
            printf("%c ", game->chessboard[i][j]);
        }
        printf("%d\n", 8 - i);
    }
    printf("  a b c d e f g h\n");
}

