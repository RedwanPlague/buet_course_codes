""" keeps some transformer functions """
from sys import exit
from queue import Queue
from main import CELL_SIZE, WIDTH_OFFSET, HEIGHT_OFFSET, ROWS, COLS, GREEN
from mygame import pygame

def board_to_screen(board_pos):
    """ takes in board position, returns pixel position in game screen """
    screen_x = board_pos[1] * CELL_SIZE + CELL_SIZE//2 + WIDTH_OFFSET
    screen_y = board_pos[0] * CELL_SIZE + CELL_SIZE//2 + HEIGHT_OFFSET
    return [screen_x, screen_y]

def screen_to_board(screen_pos):
    """ takes in screen position, returns board position """
    board_x = (screen_pos[1] - HEIGHT_OFFSET) // CELL_SIZE
    board_y = (screen_pos[0] - WIDTH_OFFSET) // CELL_SIZE
    return (board_x, board_y)

def draw_dfx_circle(surface, pos, radius, color):
    pygame.gfxdraw.aacircle(surface, pos[0], pos[1], radius, color)
    pygame.gfxdraw.filled_circle(surface, pos[0], pos[1], radius, color)

def check(piece, player):
    """ checks if this piece belongs to player """
    return piece is not None and piece.player == player

def on_board(pos):
    """ checks if pos is withing the board """
    return 0 <= pos[0] < ROWS and 0 <= pos[1] < COLS

def neighbors(pos):
    """ return neighbors of pos """
    return [
        (pos[0]+1, pos[1]+1),
        (pos[0]+1, pos[1]),
        (pos[0]+1, pos[1]-1),
        (pos[0]-1, pos[1]+1),
        (pos[0]-1, pos[1]),
        (pos[0]-1, pos[1]-1),
        (pos[0], pos[1]+1),
        (pos[0], pos[1]-1)
    ]

def bfs(board, vis, src, player, arrow = None, surface = None):
    """ runs a bfs from src """
    if surface is not None:
        draw_dfx_circle(surface, board_to_screen(src), 4, GREEN)
    que = Queue()
    que.put(src)
    vis[src[0]][src[1]] = True
    while not que.empty():
        top = que.get()
        for nei in neighbors(top):
            x, y = nei
            if on_board(nei) and check(board[x][y], player) and not vis[x][y]:
                vis[x][y] = True
                que.put(nei)
                if arrow is not None:
                    arrow(top, nei, GREEN)

def count_components(board, player, arrow = None, surface = None):
    """ find component count of player """
    count = 0
    vis = [ [False]*COLS for _ in range(ROWS) ]
    for i in range(ROWS):
        for j in range(COLS):
            if check(board[i][j], player) and not vis[i][j]:
                count += 1
                bfs(board, vis, (i, j), player, arrow, surface)
    return count

def end_game(val = 0):
    if val:
        print(-1, file=open("shared.txt", "w"), flush=True)
    exit(val)
