nohup bash -e get_pgn_data.sh 1>out.stdout 2>out.stderr &
echo $! > pid.txt
