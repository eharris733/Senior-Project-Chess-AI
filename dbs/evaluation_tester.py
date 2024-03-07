import chess.svg
import sys

def generate_html(fen, features):
    board = chess.Board(fen)
    svg = chess.svg.board(board)

    with open("evaluationTest.html", 'w') as f:
        f.write(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Chess Analysis</title>
    <style>
        body {{ font-family: Arial, sans-serif; text-align: center; }}
        .chessboard {{ margin: 20px auto; }}
        .features {{ margin: 10px; padding: 20px; border: 1px solid #ccc; border-radius: 5px; }}
    </style>
</head>
<body>

<div class="features">
    <h2>Calculated Features</h2>
    <ul>{features}</ul>
</div>

<div class="chessboard">
    {svg}
</div>

</body>
</html>
""")

if __name__ == "__main__":
    fen = sys.argv[1]
    features = sys.argv[2]  # Expecting a comma-separated list of features
    generate_html(fen, features)
