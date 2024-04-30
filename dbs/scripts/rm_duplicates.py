import csv
import random
import chess.pgn


def extract_unique_positions_with_eval_from_pgn(file_path, output_csv):
    seen_fens = set()  # To track already seen positions
    with open(file_path) as pgn, open(output_csv, mode='w', newline='') as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(['FEN', 'Evaluation'])  # Header for the CSV file

        while True:
            game = chess.pgn.read_game(pgn)
            if game is None:
                break  # End of file reached

            board = game.board()
            positions = []

            # We collect all unique positions from the game
            for move in game.mainline_moves():
                board.push(move)
                fen = board.fen()
                if fen not in seen_fens:
                    positions.append(fen)
                    seen_fens.add(fen)
                elif fen in seen_fens:
                    print(fen)

            # # If there are unique positions, choose one randomly and write its FEN along with a temporary eval to the CSV
            # if positions:
            #     random_fen = random.choice(positions)
            #     writer.writerow([random_fen, 0])  # Assigning a temporary evaluation of 0


# Example usage
file_path = '../mastergames/CarlsenAndNakamure.pgn'  # Update this with the path to your PGN file
output_csv = '../data_files/random_positions_no_duplicates.csv'
extract_unique_positions_with_eval_from_pgn(file_path, output_csv)
