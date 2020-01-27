import chess.pgn
import sys
import subprocess
from pathlib import Path
from os.path import getsize
from tqdm import tqdm
from itertools import repeat


def pgn_minify(pgn_filepath, pgn_outpath):
    # print('begin pgn_minify routine: ', end='')
    pgn_exporter = chess.pgn.FileExporter(open(pgn_outpath, 'w'), columns=None, headers=False, comments=False,
                                          variations=False)
    with open(pgn_filepath) as pgn_file, tqdm(total=getsize(pgn_filepath)) as t:
        while True:
            # print('reading game... ', end='')
            game = chess.pgn.read_game(pgn_file)
            if game is None:
                break
            # print('exporting game... ', end='')
            game.accept(pgn_exporter)
            t.update(pgn_file.tell() - t.n)
    print('done.')


if __name__ == '__main__':
    base_path = Path(sys.argv[1] if len(sys.argv) > 1 else './')

    for pgn_filepath in base_path.glob('*.pgn'):
        if pgn_filepath.stem.endswith('.minified'):
            continue

        pgn_outpath = pgn_filepath.with_name(pgn_filepath.stem + '.minified.pgn')
        pgn_minify(pgn_filepath, pgn_outpath)
        subprocess.run(['rm', str(pgn_filepath)])

    for zip_file in base_path.glob('*.bz2'):
        print(zip_file)
        subprocess.run(['lbzip2', '-d', str(zip_file)])

        pgn_filepath = zip_file.with_suffix('')
        pgn_outpath = pgn_filepath.with_name(pgn_filepath.stem + '.minified.pgn')
        pgn_minify(pgn_filepath, pgn_outpath)
        subprocess.run(['rm', str(pgn_filepath)])




