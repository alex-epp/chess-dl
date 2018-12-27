from abc import abstractmethod, ABCMeta
from typing import TYPE_CHECKING


class BoardRenderer(metaclass=ABCMeta):
    """
    Renders a chess board.
    """

    @abstractmethod
    def draw(self, fen: str, overlay):
        """
        Draw the chess board. Called every time the board
        changes.

        :param fen: FEN representation of board to draw
        :type fen: str
        :param overlay: fx added to the board
        :return: None
        """
