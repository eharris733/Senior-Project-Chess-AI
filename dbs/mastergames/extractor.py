import csv
import random
import chess.pgn

def extract_random_positions_from_pgn(file_path, output_csv):
    with open(file_path) as pgn, open(output_csv, mode='w', newline='') as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(['FEN', 'Move'])  # Header for the CSV file

        while True:

            game = chess.pgn.read_game(pgn)
            if game is None:
                break  # End of file reached

            result = game.headers["Result"]

            if result == "1-0":
                winningSide = True
            elif result == "0-1":
                winningSide = False
            else:
                continue



            board = game.board()
            positions = []  # Correctly initialize an empty list for positions
            mc = 0
            for move in game.mainline_moves():
                if game.turn() == winningSide: # only take from sides that eventually won
                    fen = board.fen()
                    board.push(move)
                    mc += 1
                    if mc > 10:
                        # Only consider positions after the 10th move to avoid opening positions
                        # and make sure the game hasn't reached an end state with too few pieces
                        if len(board.piece_map()) > 6:
                            positions.append((fen, move.uci()))

            # Check if positions list is not empty before choosing a random position
            if positions:
                random_fen, random_move = random.choice(positions)
                writer.writerow([random_fen, random_move])

# Example usage
file_path = 'CarlsenAndNakamure.pgn'
output_csv = 'ga2_all.csv'
extract_random_positions_from_pgn(file_path, output_csv)

