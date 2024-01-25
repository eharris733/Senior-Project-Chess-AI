    #include "searcher.hpp"
    #include "chess.hpp"

    int main() {
        Board board;
        board.setFen(constants::STARTPOS);
        
        
        Searcher searcher1(board);
        Searcher searcher2(board);

        // Play a game against itself with a depth of 3
        while (board.isGameOver().second == GameResult::NONE) {
            board.makeMove(searcher1.search(3));
            board.makeMove(searcher2.search(3));
            
        }


        return 0;
    }