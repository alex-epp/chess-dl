#!/bin/bash

base_path="$1"
pgn_minify="./cmake-build-release/pgn_minify"

for filename in $base_path/*.pgn.bz2; do
    bzcat filename | $pgn_minify filename && rm filename
done