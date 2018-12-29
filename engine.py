import chess
import chess.polyglot
import random
import multiprocessing
import numba


class Engine:
    def __init__(self):
        self._eval_cache = {}

    def get_move(self, board):
        _, move = self._negamax(board, 1, 4, 1 if board.turn == chess.WHITE else -1, {})
        return move

    def _negamax(self, node: chess.Board, depth, qdepth, color, cache):
        if qdepth == 0 or node.is_game_over(claim_draw=True):
            return color * self._evaluate(node), None

        node_hash = self._hash_board(node), depth

        if node_hash in cache:
            return cache[node_hash]

        # moves = list(node.legal_moves)
        # values = [color * self._evaluate(node, m) for m in moves]
        # moves = zip(*sorted(zip(values, moves), reverse=True))[0]
        moves = list(node.legal_moves)
        if depth <= 0:
            moves = sorted(random.sample(moves, k=len(moves)), key=lambda m: color*self._evaluate(node, m), reverse=True)[:2]

        best_score = None
        best_move = None
        for move in moves:
            node.push(move)
            score = -self._negamax(node, depth-1, qdepth-1, -color, cache)[0]
            if best_score is None or score >= best_score:
                best_score, best_move = score, move
            node.pop()

        cache[node_hash] = best_score, best_move
        return best_score, best_move

    @numba.jit
    def _evaluate(self, board: chess.Board, move: chess.Move = None):
        if move is not None:
            board.push(move)

        board_hash = self._hash_board(board)

        if board_hash not in self._eval_cache:
            if board.result(claim_draw=True) == '1-0':
                value = 1
            elif board.result(claim_draw=True) == '0-1':
                value = -1
            elif board.result(claim_draw=True) == '1/2-1/2':
                value = 0
            else:
                value = (
                    chess.popcount(board.pawns & board.occupied_co[chess.WHITE]) -
                    chess.popcount(board.pawns & board.occupied_co[chess.BLACK]) +
                    3 * chess.popcount((board.bishops | board.knights) & board.occupied_co[chess.WHITE]) -
                    3 * chess.popcount((board.bishops | board.knights) & board.occupied_co[chess.BLACK]) +
                    5 * chess.popcount(board.rooks & board.occupied_co[chess.WHITE]) -
                    5 * chess.popcount(board.rooks & board.occupied_co[chess.BLACK]) +
                    9 * chess.popcount(board.queens & board.occupied_co[chess.WHITE]) -
                    9 * chess.popcount(board.queens & board.occupied_co[chess.BLACK])
                ) / 160

            self._eval_cache[board_hash] = value

        if move is not None:
            board.pop()

        return self._eval_cache[board_hash]

    @numba.jit
    def _hash_board(self, board: chess.Board):
        return board.pawns, board.bishops, board.knights, board.rooks, board.queens, board.knights, board.occupied_co[chess.WHITE]


def engine_process(conn):
    engine = Engine()

    while True:
        board = chess.Board(fen=conn.recv())
        if board == 'kill':
            break

        conn.send(engine.get_move(board).uci())

    conn.close()


class EngineInterface:
    def __init__(self, opening_book: str):
        self.opening_book = chess.polyglot.open_reader(opening_book)
        self._conn, engine_conn = multiprocessing.Pipe()
        self._engine_process = multiprocessing.Process(target=engine_process, args=(engine_conn,), daemon=True)
        self._engine_process.start()

    def move(self, fen: str):
        try:
            return self.opening_book.weighted_choice(chess.Board(fen=fen)).move().uci()
        except IndexError:
            self._conn.send(fen)
            return self._conn.recv()
