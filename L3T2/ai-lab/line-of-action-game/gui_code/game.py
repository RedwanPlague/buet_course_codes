""" holds the Game class that creates the game """
from sys import stderr
from mygame import pygame
from main import ROWS, COLS, BACKG, PURPLE
from pattern import Pattern
from piece import Piece
from utility import board_to_screen, screen_to_board, on_board, count_components, end_game

DIRECTIONS = [(1, 0), (0, 1), (1, 1), (1, -1)]

class Game:
    """ for game logic """
    def __init__(self, screen, options):
        self.screen = screen
        self.types = options["types"]
        self.print = options["print"]
        self.names = options["names"]
        self.turn = 0
        self.pattern = Pattern(self.screen, options["fps"], self.draw_game)
        self.board = [ [None]*COLS for _ in range(ROWS) ]
        for j in range(1,COLS-1):
            self.board[0][j] = Piece(screen, 0, board_to_screen((0, j)))
            self.board[-1][j] = Piece(screen, 0, board_to_screen((ROWS-1, j)))
        for j in range(1,ROWS-1):
            self.board[j][0] = Piece(screen, 1, board_to_screen((j, 0)))
            self.board[j][-1] = Piece(screen, 1, board_to_screen((j, COLS-1)))

    def draw_game(self):
        """ draws the game """
        self.pattern.draw()
        for i in range(ROWS):
            for j in range(COLS):
                if self.board[i][j] is not None:
                    self.board[i][j].draw()


    def traverse_dir(self, pos, direction):
        """ returns count of pieces in that direction """
        count = 0
        cur_x, cur_y = pos
        while on_board((cur_x, cur_y)):
            if self.board[cur_x][cur_y] is not None:
                count += 1
            cur_x += direction[0]
            cur_y += direction[1]
        return count

    def try_to_move(self, pos, direction, steps, moves):
        """ takes steps in direction and if still in board, adds to moves list """
        player = self.board[pos[0]][pos[1]].player
        cur_x, cur_y = pos
        for i in range(steps):
            cur_x += direction[0]
            cur_y += direction[1]
            if not on_board((cur_x, cur_y)):
                return
            piece = self.board[cur_x][cur_y]
            if piece is not None and piece.player != player and i != steps-1:
                return
        piece = self.board[cur_x][cur_y]
        if piece is None or piece.player != player:
            moves.append((cur_x, cur_y))

    def get_all_moves(self, pos):
        """ get all valid moves of a piece at pos """
        assert self.board[pos[0]][pos[1]] is not None
        moves = []
        for direction in DIRECTIONS:
            rev_dir = -direction[0], -direction[1]
            count = self.traverse_dir(pos, direction)
            count += self.traverse_dir(pos, rev_dir) - 1
            self.try_to_move(pos, direction, count, moves)
            self.try_to_move(pos, rev_dir, count, moves)
        return moves

    def move_piece(self, piece, center, from_pos, to_pos):
        """ actually moves the piece """
        self.pattern.move(piece, center)
        self.pattern.draw_arrow(from_pos, to_pos, PURPLE)
        pygame.display.update()
        self.board[from_pos[0]][from_pos[1]] = None
        self.board[to_pos[0]][to_pos[1]] = piece
        self.turn = 1 - self.turn
        self.pattern.draw_player(self.names[self.turn], self.turn)

    def make_move(self, from_pos, to_pos):
        """ asserts whether the move is valid, then make the move """
        piece = self.board[from_pos[0]][from_pos[1]]
        assert piece is not None and piece.player == self.turn
        moves = self.get_all_moves(from_pos)
        assert to_pos in moves
        center = board_to_screen(to_pos)
        self.move_piece(piece, center, from_pos, to_pos)

    def wait_for_gui(self):
        """ takes gui input from player """
        waiting = True
        piece = None
        from_pos = (None, None)
        moves = []
        while waiting:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    end_game(1)
                elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                    pos = screen_to_board(event.pos)
                    if not on_board(pos):
                        continue
                    if pos in moves:
                        when = self.print[self.turn]
                        f = open("shared.txt", "w")
                        f.write("%d\n" % self.turn)
                        if when == 'Y':
                            f.write("%d %d %d %d\n" % (from_pos[0], from_pos[1], pos[0], pos[1]))
                            print(from_pos[0], from_pos[1], pos[0], pos[1], file=stderr, flush=True)
                        self.move_piece(piece, board_to_screen(pos), from_pos, pos)
                        if when == 'y':
                            f.write("%d %d %d %d\n" % (from_pos[0], from_pos[1], pos[0], pos[1]))
                            print(from_pos[0], from_pos[1], pos[0], pos[1], file=stderr, flush=True)
                        f.close()
                        waiting = False
                    else:
                        piece = self.board[pos[0]][pos[1]]
                        from_pos = pos
                        if piece is not None and piece.player == self.turn:
                            moves = self.get_all_moves(pos)
                            self.draw_game()
                            self.pattern.highlight(pos, moves)
                        else:
                            moves = []
                            self.draw_game()
                        pygame.display.update()

    def wait_for_input(self):
        """ waits for input from console """
        done = False
        while not done:
            f = open("shared.txt", "r")
            line = f.readline()
            if line == '':
                continue
            who = int(line)
            if who == 2:
                line = f.readline()
                f.close()
                from_x, from_y, to_x, to_y = map(int, line.split())
                who = self.turn
                self.make_move((from_x, from_y), (to_x, to_y))
                if self.print[who] == 'y':
                    f = open("shared.txt", "w")
                    f.write("%d\n" % who)
                    f.write("%d %d %d %d\n" % (from_x, from_y, to_x, to_y))
                    f.close()
                    print(from_x, from_y, to_x, to_y, file=stderr, flush=True)
                done = True
            else:
                f.close()
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    end_game(1)

    def run_game(self):
        """ runs the game """
        self.screen.fill(BACKG)
        self.pattern.draw_player(self.names[self.turn], self.turn)
        self.pattern.draw_labels()
        self.draw_game()
        pygame.display.update()

        winner = -1
        while winner == -1:
            if self.types[self.turn] == 'h':
                self.wait_for_gui()
            else:
                self.wait_for_input()
            if count_components(self.board, 1-self.turn) == 1:
                winner = 1 - self.turn
            elif count_components(self.board, self.turn) == 1:
                winner = self.turn

        self.draw_game()
        count_components(self.board, winner, self.pattern.draw_arrow, self.screen)
        self.pattern.draw_player(self.names[winner], winner, True)
        pygame.display.update()

        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    end_game()
