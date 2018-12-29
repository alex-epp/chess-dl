import config
from sdl_window import SDLWindow, SDLSpritePack, SDLDragger
from sdl_board_renderer import SDLBoardRenderer

from state import State
from engine import EngineInterface

import chess
import pygame
import threading


class App:
    PLAYER_TURN = 0
    AI_TURN = 1
    GAME_OVER = 2
    MENU = 3

    def __init__(self):
        self._engine = EngineInterface(opening_book=config.opening_book)

        self._sprites = SDLSpritePack.from_file(config.sprite_pack)
        self._window = SDLWindow(config.window_config)
        self._renderer = SDLBoardRenderer(self._sprites, self._window)
        self._overlay = {}

        self._board = chess.Board()

        self._state = State(self.PLAYER_TURN)
        self._state.register_on_change(self._state_change_listener)

        self._dragger = SDLDragger(self._renderer)
        self._window.register_event_listener(self._dragger.process_event)
        self._dragger.register_event_listener(self._drag_event_listener)

    def tick(self):
        self._renderer.draw(self._board.fen(), self._overlay)
        return self._handle_events()

    def _drag_event_listener(self, event: SDLDragger.Event):
        if event.type == SDLDragger.Event.DRAG:
            self._overlay['drag'] = event.drag
        elif event.type == SDLDragger.Event.START:
            self._overlay['drag_start'] = event.start
            self._overlay['drag'] = event.drag
            if self._state.is_at(self.PLAYER_TURN):
                self._overlay['highlight'] = [
                    self._renderer.Coord(uci=move.uci()[2:])
                    for move in self._board.legal_moves
                    if move.uci()[:2] == self._overlay['drag_start'].uci
                ]
        elif event.type == SDLDragger.Event.END:
            self._overlay.pop('drag_start', None)
            self._overlay.pop('highlight', None)
            if self._state.is_at(self.PLAYER_TURN):
                move = chess.Move.from_uci(event.start.uci + event.end.uci)
                if self._board.is_legal(move):
                    self._make_player_move(move)

    def _make_move(self, move):
        self._board.push(move)
        self._overlay['last_move'] = (self._renderer.Coord(uci=move.uci()[:2]),
                                      self._renderer.Coord(uci=move.uci()[2:]))

    def _make_player_move(self, move):
        self._make_move(move)
        if self._board.is_game_over(claim_draw=True):
            self._state.change_to(self.GAME_OVER)
        else:
            self._state.change_to(self.AI_TURN)

    def _engine_move_thread(self):
        self._make_move(chess.Move.from_uci(self._engine.move(self._board.fen())))
        if self._board.is_game_over(claim_draw=True):
            self._state.change_to(self.GAME_OVER)
        else:
            self._state.change_to(self.PLAYER_TURN)

    def _make_engine_move(self):
        self._engine_thread = threading.Thread(target=self._engine_move_thread, daemon=True)
        self._engine_thread.start()

    def _state_change_listener(self, from_state, to_state):
        if from_state == self.MENU:
            self._dragger.enable()
            self._overlay.pop('menu', None)

        if to_state == self.MENU:
            self._dragger.disable()

        if to_state == self.AI_TURN:
            self._make_engine_move()

    def _handle_events(self):
        done = False
        for event in self._window.get_events():
            if event.type == pygame.QUIT:
                done = True
            elif event.type == pygame.MOUSEMOTION:
                if self._state.is_at(self.PLAYER_TURN) or self._state.is_at(self.AI_TURN):
                    self._overlay['hover'] = self._renderer.Coord(screen=event.pos)

        return not done


def main():
    app = App()

    done = False
    while not done:
        done = not app.tick()


if __name__ == '__main__':
    main()
