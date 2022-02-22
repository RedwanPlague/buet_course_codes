""" holds the Piece class """
from main import PIECE_COLORS, PIECE_RADIUS, PIECE_OUTLINE
from utility import draw_dfx_circle

class Piece:
    """ a piece for the game """
    def __init__(self, surface, player, center):
        self.surface = surface
        self.player = player
        self.center = center

    def __str__(self):
        return str(self.player) + " - " + str(self.center)

    def draw(self):
        """ draws the piece on the board """
        draw_dfx_circle(self.surface, self.center, PIECE_OUTLINE, PIECE_COLORS[1-self.player])
        draw_dfx_circle(self.surface, self.center, PIECE_RADIUS, PIECE_COLORS[self.player])

    def get_diff(self, point):
        """ returns differect vector of point from center """
        return point[0] - self.center[0], point[1] - self.center[1]

    def move(self, vect):
        """ moves the center towards vect """
        self.center[0] += vect[0]
        self.center[1] += vect[1]
