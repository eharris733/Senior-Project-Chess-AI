import csv
import chess
import chess.engine


def is_within_range(scores, threshold=20):
    """
    Check if all scores are within the specified threshold (in centipawns).
    """
    min_score = min(scores)
    max_score = max(scores)
    return (max_score - min_score) <= threshold


def evaluate_positions(input_csv, output_csv, engine_path):
    """
    Evaluate positions using Stockfish's multi-PV mode. Only add positions to the output CSV if all top 4 move
    evaluations are within 20 centipawns of each other.
    """
    with open(input_csv, newline='') as infile, open(output_csv, 'w', newline='') as outfile, \
            chess.engine.SimpleEngine.popen_uci(engine_path) as engine:
        reader = csv.reader(infile)
        writer = csv.writer(outfile)

        for row in reader:
            fen = row[0]
            board = chess.Board(fen)

            # Set Stockfish to use multi-PV mode


            # Analyze the position
            info = engine.analyse(board, chess.engine.Limit(depth=8), multipv=4)
            scores = [item['score'].white().score(mate_score=100000) for item in info if 'score' in item]

            # Check if all evaluations are within 20 centipawns
            if len(scores) == 4 and is_within_range(scores, 50):
                writer.writerow([fen])


# Example usage
input_csv = 'fen_cp_evaluations_20000.csv'  # Path to your input CSV file with FEN strings
output_csv = 'qs.csv'  # Path to the output CSV file
engine_path = '/usr/local/Cellar/stockfish/16/bin/stockfish'  # Path to your Stockfish engine executable
evaluate_positions(input_csv, output_csv, engine_path)
