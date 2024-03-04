from time import sleep

import chess
import chess.engine
import csv

# Adjust the path to where your Stockfish engine is located
STOCKFISH_PATH = '/usr/local/Cellar/stockfish/16/bin/stockfish'
EPD_FILE_PATH = 'quiet-labeled.v7.epd'
CSV_FILE_PATH = 'quiet_evals.csv'
N = 13000  # Number of lines to read and process

def read_positions_from_epd(epd_file_path):
    positions = []
    with open(epd_file_path, 'r') as file:
        curr = 0
        for line in file:
            if curr > N:
                return positions
            parts = line.strip().split('c9')
            fen = parts[0].strip()  # Assuming the first part is the FEN string
            positions.append(fen)
            curr += 1
    return positions

def evaluate_positions(positions, stockfish_path, csv_file_path):
    with chess.engine.SimpleEngine.popen_uci(stockfish_path) as engine, open(csv_file_path, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['FEN', 'Evaluation'])
        for fen in positions:
            try:
                board = chess.Board(fen)
                info = engine.analyse(board, chess.engine.Limit(time=1))  # Set the analysis time limit to 0.1 seconds
                evaluation = info['score'].white().score(mate_score=10000)
                if abs(evaluation) < 9000:  # exclude mate scores
                    if len(board.piece_map()) > 6:  # exclude endgames that are solved
                        writer.writerow([fen, evaluation])
            except Exception as e:
                print(f"Error processing FEN: {fen}. Error: {e}")

def main():
    positions = read_positions_from_epd(EPD_FILE_PATH)
    evaluate_positions(positions, STOCKFISH_PATH, CSV_FILE_PATH)
    print("Processing complete.")

if __name__ == "__main__":
    main()