import csv
import chess

def should_filter_out(fen, eval_score, max_eval=1000, min_pieces=7):
    """Determine if a position should be filtered out based on the evaluation and number of pieces."""
    board = chess.Board(fen)
    num_pieces = len(board.piece_map())
    return abs(eval_score) > max_eval or num_pieces < min_pieces

def process_csv(input_csv_path, output_csv_path):
    with open(input_csv_path, mode='r', newline='') as input_file, open(output_csv_path, mode='w', newline='') as output_file:
        reader = csv.reader(input_file)
        writer = csv.writer(output_file)

        # Assuming the first row is the header
        header = next(reader)
        writer.writerow(header)  # Write the header to the output file

        for row in reader:
            fen, eval_str = row
            eval_score = int(eval_str)  # Convert eval to integer

            if not should_filter_out(fen, eval_score):
                writer.writerow(row)

input_csv_path = '../data_files/quiet_evals.csv'  # Update this path
output_csv_path = '../data_files/quiet_evals_filtered.csv'  # Update this path

process_csv(input_csv_path, output_csv_path)