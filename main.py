import config
from sdl_window import SDLWindow, SDLSpritePack, SDLDragger
from sdl_board_renderer import SDLBoardRenderer

from engine import Engine

import chess
import pygame
import random


def update_overlay(overlay, renderer, board, event):
    if event.type == SDLDragger.Event.DRAG:
        overlay['drag'] = event.drag
    elif event.type == SDLDragger.Event.START:
        overlay['drag_start'] = event.start
        overlay['drag'] = event.drag
        overlay['highlight'] = [
            renderer.Coord(uci=move.uci()[2:])
            for move in board.legal_moves if move.uci()[:2] == overlay['drag_start'].uci
        ]
    elif event.type == SDLDragger.Event.END:
        overlay.pop('drag_start')
        overlay.pop('highlight', None)


def move(board, event):
    if event.type == SDLDragger.Event.END:
        m = chess.Move.from_uci(event.start.uci + event.end.uci)
        if board.is_legal(m):
            board.push(m)


def main():
    engine = Engine(opening_book='Assets/gm2001.bin')

    sprites = SDLSpritePack.from_file('Assets/sprites.spritepack')
    window = SDLWindow(config.WindowConfig)
    renderer = SDLBoardRenderer(sprites, window)
    overlay = {}

    board = chess.Board()

    dragger = SDLDragger(renderer)
    window.register_event_listener(dragger.process_event)

    dragger.register_event_listener(lambda event: update_overlay(overlay, renderer, board, event))
    dragger.register_event_listener(lambda event: move(board, event))

    done = False
    while not done:
        renderer.draw(board.fen(), overlay)

        if board.turn == chess.BLACK and not board.is_game_over(claim_draw=True):
            board.push(chess.Move.from_uci(engine.move(board.fen())))

        for event in window.get_events():
            if event.type == pygame.QUIT:
                done = True


if __name__ == '__main__':
    main()
