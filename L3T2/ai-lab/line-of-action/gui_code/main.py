""" actually starts the game """
import sys

if len(sys.argv) != 11:
    sys.exit("Please provide game option args, expected {}, got {}".format(10, len(sys.argv)-1))

ROWS = int(sys.argv[1])
COLS = int(sys.argv[2])
CELL_SIZE = 50
WIDTH_OFFSET = 30
HEIGHT_OFFSET = 90
SCREEN_WIDTH = max(COLS * CELL_SIZE + WIDTH_OFFSET * 2, 300)
SCREEN_HEIGHT = ROWS * CELL_SIZE + HEIGHT_OFFSET + WIDTH_OFFSET
PIECE_OUTLINE = CELL_SIZE * 2 // 5
PIECE_RADIUS = PIECE_OUTLINE - 2
BLACK = 0, 0, 0
WHITE = 255, 255, 255
# BACKG = 144, 238, 144
BACKG = 170, 170, 170
RED = 255, 0, 0
GREEN = 0, 255, 0
PURPLE = 0, 0, 255
BOARD_COLORS = [(255, 146, 8), (255, 195, 120)]
PIECE_COLORS = [(0, 0, 0), (255, 255, 255)]

def main():
    """ the main function """
    from mygame import pygame
    from game import Game

    pygame.init()

    screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption(sys.argv[7])

    options = {
        "types": (sys.argv[3], sys.argv[4]),
        "print": (sys.argv[5], sys.argv[6]),
        "names": (sys.argv[8], sys.argv[9]),
        "fps": int(sys.argv[10])
    }
    game = Game(screen, options)
    game.run_game()

    pygame.quit()

if __name__ == "__main__":
    main()
