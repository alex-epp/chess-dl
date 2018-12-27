from board_renderer import BoardRenderer
from sdl_window import SDLWindow, SDLSpritePack
from coord import TiledBoardRenderCoord

import pygame
import numpy as np


class SDLBoardRenderer(BoardRenderer):
    def __init__(self, sprite_pack:  SDLSpritePack, window: SDLWindow):
        self._sprite_pack = sprite_pack
        self._window = window

        self._sprite_pack.load()
        self._board = self._create_board_surface()
        self._pieces = self._create_pieces_surface()
        self._screen = pygame.Surface(self._board.get_size())
        self._window.backbuffer_size = self._board.get_size()

    def Coord(self, *, screen=None, tile=None, rank_file=None, uci=None):
        return TiledBoardRenderCoord(self, screen=screen, tile=tile, rank_file=rank_file, uci=uci)

    def _create_board_surface(self):
        board = pygame.Surface(self._sprite_pack.tile_size * 8)
        for x in range(8):
            for y in range(8):
                tile = 'white' if (x % 2 == y % 2) else 'black'
                pos = self._sprite_pack.tile_size * (x, y)
                self._sprite_pack.blit(tile, board, pos)

        return board

    def _create_pieces_surface(self) -> pygame.Surface:
        pieces = pygame.Surface(self._board.get_size(), pygame.SRCALPHA)
        return pieces.convert_alpha()

    def _extract_piece_map(self, fen):
        piece_map = {}

        rank, file = 8, 1
        for char in fen:
            if char == '/':
                rank -= 1
                file = 1
            elif char in ('1', '2', '3', '4', '5', '6', '7', '8'):
                file += int(char)
            elif char == ' ':
                break  # Ignore special indicator values
            else:
                piece_map[self.Coord(rank_file=(rank, file))] = char
                file += 1

        return piece_map

    def _blit_pieces(self, piece_map, overlay):
        self._pieces.fill((0, 0, 0, 0))

        # Render pieces
        for coord, tile in piece_map.items():
            self._sprite_pack.blit(tile, self._pieces, coord.screen)

        # Render highlights
        if 'highlight' in overlay:
            for coord in overlay['highlight']:
                self._sprite_pack.blit('highlight', self._pieces, coord.screen)

        # Render dragging
        if 'drag_start' in overlay:
            drag_start = overlay['drag_start']
            drag_coord = overlay['drag']

            try:
                tile = next(tile for coord, tile in piece_map.items() if (coord.rank_file == drag_start.rank_file).all())
                self._sprite_pack.blit(tile, self._pieces, drag_coord.screen - drag_start.screen + self.Coord(rank_file=drag_start.rank_file).screen)
            except StopIteration:
                pass

    def draw(self, fen: str, overlay):
        self._blit_pieces(self._extract_piece_map(fen), overlay)

        self._window.clear()
        self._window.draw(self._board, (0, 0))
        self._window.draw(self._pieces, (0, 0))
        self._window.flip()

    def _screen_pos_to_tile_pos(self, pos):
        return np.array((pos[0], self._board.get_size()[1] - pos[1])) // self._sprite_pack.tile_size

    def _rank_file_to_uci(self, rank_file):
        rank, file = rank_file
        return '{file}{rank}'.format(file=chr(ord('a') + int(file) - 1), rank=int(rank))

    def _tile_pos_to_rank_file(self, pos):
        # Depends on what orientation the board is shown in
        return pos[::-1] + 1

    def _uci_to_rank_file(self, uci):
        return np.array((int(uci[1]), (ord(uci[0]) - ord('a') + 1)))

    def _rank_file_to_tile_pos(self, rank_file):
        # Depends on what orientation the board is shown in
        return rank_file[::-1] - 1

    def _tile_pos_to_screen_pos(self, pos):
        pos = pos * self._sprite_pack.tile_size
        pos[1] = self._board.get_size()[1] - pos[1] - self._sprite_pack.tile_size[1]
        return pos
