""" holds the Pattern class """
from mygame import pygame
from main import ROWS, COLS, CELL_SIZE, BOARD_COLORS, RED, BLACK, BACKG, WIDTH_OFFSET, HEIGHT_OFFSET
from utility import board_to_screen, draw_dfx_circle

FONT = "Monospace"

class Pattern:
    """ board pattern for the game """
    def __init__(self, surface, fps, background):
        self.surface = surface
        self.fps = fps
        self.background = background
        self.prev = None

    def draw(self):
        """ draws the board pattern """
        for row in range(ROWS):
            for col in range(COLS):
                top, left = board_to_screen((row, col))
                rect = top - CELL_SIZE//2, left - CELL_SIZE//2, CELL_SIZE, CELL_SIZE
                pygame.draw.rect(self.surface, BOARD_COLORS[(row + col)%2], rect)

    def draw_player(self, name, turn, done = False):
        """ draws the player names and colors """
        font = pygame.font.SysFont(FONT, 20, True)
        # color = BLACK if turn == 0 else WHITE
        color = BLACK
        color_txt = "black" if turn == 0 else "white"
        pref = "turn: " if not done else "winner: "
        text = font.render(pref + color_txt + "(" + name + ")", True, color)
        self.surface.fill(BACKG, self.prev)
        self.prev = self.surface.blit(text, (WIDTH_OFFSET, 10))
        pygame.display.update()

    def draw_labels(self):
        """ draws the labels """
        font = pygame.font.SysFont(FONT, 18, True)
        for i in range(ROWS):
            num = chr(ord('0')+(ROWS-i))
            text = font.render(num, True, BLACK)
            height =  CELL_SIZE*i + HEIGHT_OFFSET + 16
            self.surface.blit(text, (10, height))
            self.surface.blit(text, (COLS * CELL_SIZE + WIDTH_OFFSET + 8, height))
        for i in range(COLS):
            num = chr(ord('A')+i)
            text = font.render(num, True, BLACK)
            self.surface.blit(text, (CELL_SIZE*i + 50, HEIGHT_OFFSET - 24))
            self.surface.blit(text, (CELL_SIZE*i + 50, HEIGHT_OFFSET + ROWS * CELL_SIZE + 6))

    def draw_arrow(self, from_pos, to_pos, color = RED):
        """ draws arrow from 'pos' to 'to' """
        from_pos = board_to_screen(from_pos)
        to_pos = board_to_screen(to_pos)
        draw_dfx_circle(self.surface, to_pos, 4, color)
        if not (from_pos[0] == to_pos[0] or from_pos[1] == to_pos[1]): # not axis parallel
            pygame.draw.line(self.surface, color, from_pos, to_pos, 3)
        else:
            pygame.draw.line(self.surface, color, from_pos, to_pos, 2)

    def highlight(self, pos, moves):
        """ highlights possible moves """
        for move in moves:
            self.draw_arrow(pos, move)

    def move(self, piece, center):
        """ moves the piece across the board """
        diff_x, diff_y = piece.get_diff(center)
        speed_x = 1 if diff_x > 0 else -1 if diff_x < 0 else 0
        speed_y = 1 if diff_y > 0 else -1 if diff_y < 0 else 0
        assert diff_x == 0 or diff_y == 0 or abs(diff_x/diff_y) == 1
        prev = piece.center
        clock = pygame.time.Clock()
        while piece.center != center:
            clock.tick(self.fps)
            piece.move((speed_x, speed_y))
            self.background()
            piece.draw()
            pygame.display.update()
