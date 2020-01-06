import chess.pgn
import sys
import subprocess
from pathlib import Path

if __name__ == '__main__':
    base_path = Path(sys.argv[1] if len(sys.argv) > 1 else './')

    for zip_file in base_path.glob('*.bz2'):
        print(zip_file)
        subprocess.run(['lbzip2', '-d', str(zip_file)])
        pgn_filepath = zip_file.with_suffix('')
        pgn_outpath = pgn_filepath.with_name(pgn_filepath.name + '_minified')
        pgn_exporter = chess.pgn.FileExporter(open(pgn_outpath), columns=None, headers=False, comments=False, variations=False)

        with open(pgn_filepath) as pgn_file:
            while True:
                game = chess.pgn.read_game(pgn_file)
                if game is None:
                    break
                game.export(pgn_exporter)

        subprocess.run(['rm', str(pgn_filepath)])
