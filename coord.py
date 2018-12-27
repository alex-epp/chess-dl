import numpy as np


class TiledBoardRenderCoord:
    def __init__(self, board_renderer, *, screen=None, tile=None, rank_file=None, uci=None):
        self._board_renderer = board_renderer
        self._screen = np.array(screen) if screen is not None else None
        self._tile = np.array(tile, dtype=int) if tile is not None else None
        self._rank_file = np.array(rank_file, dtype=int) if rank_file is not None else None
        self._uci = str(uci) if uci is not None else None

    @classmethod
    def build_screen(cls, coord: 'TiledBoardRenderCoord'):
        return cls(coord._board_renderer, screen=coord.screen)

    @classmethod
    def build_tile(cls, coord: 'TiledBoardRenderCoord'):
        return cls(coord._board_renderer, tile=coord.tile)

    @classmethod
    def build_rank_file(cls, coord: 'TiledBoardRenderCoord'):
        return cls(coord._board_renderer, rank_file=coord.rank_file)

    @classmethod
    def build_uci(cls, coord: 'TiledBoardRenderCoord'):
        return cls(coord._board_renderer, uci=coord.uci)

    @property
    def screen(self):
        if self._screen is not None:
            return self._screen

        return self._board_renderer._tile_pos_to_screen_pos(self.tile)

    @property
    def tile(self):
        if self._tile is not None:
            return self._tile

        if self._screen is not None:
            return self._board_renderer._screen_pos_to_tile_pos(self.screen)
        else:
            return self._board_renderer._rank_file_to_tile_pos(self.rank_file)

    @property
    def rank_file(self):
        if self._rank_file is not None:
            return self._rank_file

        if self._uci is not None:
            return self._board_renderer._uci_to_rank_file(self._uci)
        else:
            return self._board_renderer._tile_pos_to_rank_file(self.tile)

    @property
    def uci(self):
        if self._uci is not None:
            return self._uci

        return self._board_renderer._rank_file_to_uci(self.rank_file)
