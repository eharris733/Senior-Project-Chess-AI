import csv
import random
import chess.pgn


def extract_random_positions_from_pgn(file_path, output_csv):
    with open(file_path) as pgn, open(output_csv, mode='w', newline='') as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(['FEN'])  # Header for the CSV file

        while True:
            game = chess.pgn.read_game(pgn)
            if game is None:
                break  # End of file reached

            # Extract all positions from the game
            board = game.board()
            positions = [board.fen()]
            mc = 0
            for move in game.mainline_moves():
                board.push(move)
                mc += 1
                if(mc > 10):
                    positions.append(board.fen())

            # Choose a random position and write its FEN to the CSV
            random_fen = random.choice(positions)
            writer.writerow([random_fen])


# Example usage
file_path = 'Nakamura.pgn'  # Update this with the path to your PGN file
output_csv = 'random_positions.csv'
extract_random_positions_from_pgn(file_path, output_csv)
