import csv
import json
import chess

import zstandard as zstd
import io

# Define the number of lines you want to read
num_lines_to_read = 100000  # For example, read only the first 1000 lines

# Create a Zstandard decompressor
decompressor = zstd.ZstdDecompressor()


# this parses the json and gets me the only two values we care about
# the fen and the final most accurate evaluation
def parse_deepest_eval(line):
    valid_entry = False
    # Convert the JSON string into a Python dictionary
    data = json.loads(line)

    # Extract the FEN string
    fen = data['fen']

    # Initialize variables to find the deepest eval
    deepest_eval = None
    max_depth = -1

    # Iterate through each eval to find the one with the deepest depth
    for eval in data['evals']:
        if eval['depth'] > max_depth:
            max_depth = eval['depth']
            # Assuming you want the first pv of the deepest eval
            deepest_eval = eval['pvs'][0]  # First pv of the deepest eval

    # Extract the cp value and line from the deepest eval
    # check to see if a cp exists and is not a mate pos

    if 'cp' in deepest_eval:
        cp = deepest_eval['cp']
        board = chess.Board(fen)
        # if it's an extreme position or is check we won't be able to search it very well
        if abs(cp) > 500 or board.is_check():
            return False
        return fen, cp

    return False


# Open the output CSV file for writing
with open("fen_cp_evaluations.csv", "w", newline='') as csvfile:
    csvwriter = csv.writer(csvfile)
    csvwriter.writerow(["FEN", "Evaluation"])  # Write the header

    # Open the .zst file
    with open("lichess_db_eval.json.zst", "rb") as compressed:
        with decompressor.stream_reader(compressed) as decompressed_stream:
            text_stream = io.TextIOWrapper(decompressed_stream, encoding='utf-8')
            line_count = 0

            for line in text_stream:
                result = parse_deepest_eval(line)
                if result:
                    # Write the FEN and cp value to the CSV
                    csvwriter.writerow(result)
                    line_count += 1
                    if line_count >= num_lines_to_read:
                        break

print(f"{line_count} entries written to fen_cp_evaluations.csv")
