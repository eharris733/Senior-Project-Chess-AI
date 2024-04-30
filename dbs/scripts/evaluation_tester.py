import chess.svg
import sys


def generate_html(fen, features):
    board = chess.Board(fen)
    svg = chess.svg.board(board)

    # Split the features string by newline and create list items for each feature
    features_list = features.splitlines()
    features_html = ''.join(f"<li>{feature.strip()}</li>" for feature in features_list)

    with open("evaluationTest.html", 'w') as f:
        f.write(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Chess Analysis</title>
    <style>
        body {{ font-family: Arial, sans-serif; text-align: center; }}
        .container {{ overflow: hidden; }}
        .chessboard, .features {{ float: left; margin: 20px; }}
        .chessboard {{ width: 600px; }} /* Adjust based on the actual size of your SVG */
        .features {{ width: 500px; padding: 20px; border: 1px solid #ccc; border-radius: 5px; }}
    </style>
</head>
<body>

<div class="container">
    <div class="features">
        <h2>Calculated Features</h2>
        <ul>{features_html}</ul>
    </div>

    <div class="chessboard">
        {svg}
    </div>
</div>

</body>
</html>
""")


if __name__ == "__main__":
    fen = sys.argv[1]
    features = sys.argv[2]  # Expecting a newline-separated list of features
    generate_html(fen, features)
