import chess
import chess.polyglot
import random
import math


class Engine:
    def __init__(self, opening_book: str):
        self.opening_book = chess.polyglot.open_reader(opening_book)
        self._eval_cache = {}

    def move(self, fen: str):
        board = chess.Board(fen=fen)

        try:
            move = self.opening_book.weighted_choice(board).move()
        except IndexError:
            move = self._engine_move(board)

        return move.uci()

    def _engine_move(self, board):
        best_value = -math.inf
        best_move = None

        cache = {}
        is_maximizing = (board.turn == chess.WHITE)
        for move in sorted(board.legal_moves, key=lambda m: self._evaluate(board, m), reverse=is_maximizing)[:3]:
            value = self._minimax(board, move, cache, 2, not is_maximizing)
            if board.turn == chess.BLACK:
                value *= -1
            if value > best_value:
                best_move = move
                best_value = value

        return best_move

    def _negamax(self, node: chess.Board, depth, color, cache):
        if depth == 0 or node.is_game_over(claim_draw=True):
            return self._evaluate(node)

        if (node.__hash__(), depth) in cache:
            return cache[(node.__hash__(), depth)]

        moves = sorted(node.legal_moves, key=lambda m: color*self._evaluate(node, m), reverse=True)

    def _evaluate(self, board: chess.Board, move: chess.Move = None):
        if move is not None:
            board.push(move)

        if board.__hash__() not in self._eval_cache:
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
                    3 * chess.popcount(board.bishops & board.knights & board.occupied_co[chess.WHITE]) -
                    3 * chess.popcount(board.pawns & board.knights & board.occupied_co[chess.BLACK]) +
                    5 * chess.popcount(board.rooks & board.occupied_co[chess.WHITE]) -
                    5 * chess.popcount(board.rooks & board.occupied_co[chess.BLACK]) +
                    9 * chess.popcount(board.queens & board.occupied_co[chess.WHITE]) -
                    9 * chess.popcount(board.queens & board.occupied_co[chess.BLACK])
                ) / 160

            self._eval_cache[board.__hash__()] = value

        if move is not None:
            board.pop()

        return self._eval_cache[board.__hash__()]
