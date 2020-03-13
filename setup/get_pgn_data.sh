base_path_1="/mnt/lichess_1"
base_path_2="/mnt/lichess_2"

urls_1="./pgn_urls_1"
urls_2="./pgn_urls_2"

pgn_minify="../cmake-build-release/setup/pgn_minify"

filename_pattern="lichess_db_standard_rated_.*.pgn"

while read -r url; do
  filename=$(echo "$url" | grep -o "$filename_pattern")
  outfile="$base_path_1"/"$filename"
  if [ ! -f "$outfile" ]; then
    wget -O - "$url" | lbzip2 -d | $pgn_minify >"$outfile".tmp
    mv "$outfile".tmp "$outfile"
  fi
done < $urls_1

while read -r url; do
  filename=$(echo "$url" | grep -o "$filename_pattern")
  outfile="$base_path_2"/"$filename"
  if [ ! -f "$outfile" ]; then
    wget -O - "$url" | lbzip2 -d | $pgn_minify >"$outfile".tmp
    mv "$outfile".tmp "$outfile"
  fi
done < $urls_2
