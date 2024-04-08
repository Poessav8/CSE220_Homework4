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

bool is_valid_rook_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
   
    // Verify that rook is not moving diagonally, i.e., both row and column change
    if (src_row != dest_row && src_col != dest_col) {
        return false;
    }
    if(src_row == dest_row && src_col == dest_row){ //rook doesn't move
	    return false;
	}

    // Verify that no pieces are blocking the path between source and destination.
    if (src_row == dest_row) { // Rows stay the same, column changes
        int start = (src_col > dest_col) ? dest_col : src_col;
        int end = (src_col > dest_col) ? src_col : dest_col;
        for (int i = start + 1; i < end; i++) {
            if (game->chessboard[src_row][i] != '.') {
                return false;
            }
        }
    } else if (src_col == dest_col) { // Columns stay the same, row changes
        int start = (src_row > dest_row) ? dest_row : src_row;
        int end = (src_row > dest_row) ? src_row : dest_row;
        for (int i = start + 1; i < end; i++) {
            if (game->chessboard[i][src_col] != '.') {
                return false;
            }
        }
    } else { // Rook is not moving along either rows or columns
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

bool is_valid_bishop_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
      //verifies that bishop is not moving solely vertically or horizontally
    if(src_row == dest_row || src_col == dest_col){
	    return false;
	   }
	  //verify that it's moving diagonally correctly 
	  if(abs(src_row - dest_row) != abs(src_col - dest_col)){
		  return false;
		}

    // verify that there are no pieces blocking path between positions
    if (dest_col > src_col) { // moved right, now can move up or down
        if (dest_row > src_row) { // moved down
            int i = src_row + 1;
            int j = src_col + 1;
            while (i < dest_row) {
                if (game->chessboard[i][j] != '.') {
                    return false;
                }
                i++, j++;
            }
        } else { // moved up !! INPUT goes here, dest_row < src_row
            int i = src_row - 1; // = 4... huh. wrong
            int j = src_col + 1; // = 
            while (i > dest_row) {
                if (game->chessboard[i][j] != '.') {
                    return false;
                }
                i--, j++;
            }
        }
    } else{ //moved left , so end_col < start_col
		if (dest_row > src_row) { // moved down
            int i = src_row + 1;
            int j = src_col - 1; // Corrected here
            while (i < dest_row) {
                if (game->chessboard[i][j] != '.') {
                return false;
            }
            i++, j--;
        }
	    } else { // moved up
        int i = src_row - 1;
        int j = src_col - 1; // Corrected here
            while (i > dest_row) {
                if (game->chessboard[i][j] != '.') {
                    return false;
                }
                i++, j--;
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
    while (fen[i] != ' ') {
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

    if(from_row < '0' || from_row > '8' || to_row < '0' || to_row > '8'){
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
    (void)game;
    (void)move;
    (void)is_client;
    (void)validate_move;
    return -999;
}

int send_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    (void)game;
    (void)message;
    (void)socketfd;
    (void)is_client;
    return -999;
}

int receive_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    (void)game;
    (void)message;
    (void)socketfd;
    (void)is_client;
    return -999;
}

int save_game(ChessGame *game, const char *username, const char *db_filename) {
    (void)game;
    (void)username;
    (void)db_filename;
    return -999;
}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {
    (void)game;
    (void)username;
    (void)db_filename;
    (void)save_number;
    return -999;
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

