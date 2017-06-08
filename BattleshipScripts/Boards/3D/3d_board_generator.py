import os
import random


class Ship:
    def __init__(self, size, score, letter):
        self.size = size
        self.score = score
        self.letter = letter


class Board:
    def __init__(self, r, c, d):
        self._board = [[[' ' for k in range(c)] for j in range(r)] for i in range(d)]
        self.rows = r
        self.cols = c
        self.depth = d

    def is_in_board(self, r, c, d):
        return 0 <= r < self.rows and 0 <= c < self.cols and 0 <= d < self.depth

    def set(self, coord, val):
        r, c, d = coord[0], coord[1], coord[2]
        if self.is_in_board(r, c, d):
            self._board[d][r][c] = val
            return True
        return False

    def get(self, r, c, d):
        return self._board[d][r][c]

    def get_adjacent_coordinates(self, r, c, d):
        increments = [(1, 0, 0), (-1, 0, 0),
                      (0, 1, 0), (0, -1, 0),
                      (0, 0, 1), (0, 0, -1)]

        coords = set()
        for inc in increments:
            row = r + inc[0]
            col = c + inc[1]
            depth = d + inc[2]
            if self.is_in_board(row, col, depth):
                coords.add((row, col, depth))

        return coords

    def get_all_legal_coordinates(self):
        allCoordinates = {(i, j, k) for i in range(self.rows) for j in range(self.cols) for k in range(self.depth)}
        coordsToRemove = set()

        for coord in allCoordinates:
            if self.get(coord[0], coord[1], coord[2]) != ' ':
                coordsToRemove = coordsToRemove.union(self.get_adjacent_coordinates(coord[0], coord[1], coord[2]))
                coordsToRemove.add(coord)

        for cRemove in coordsToRemove:
            allCoordinates.discard(cRemove)

        return allCoordinates

    def set_ship_random(self, ship):
        legalCoords = self.get_all_legal_coordinates()
        start = random_element(legalCoords)
        if ship.size == 1:
            return self.set(start, ship.letter)
        allAdjacent = self.get_adjacent_coordinates(start[0], start[1], start[2])
        next = random_element(allAdjacent)
        if next not in legalCoords:
            return False
        direction = (next[0] - start[0], next[1] - start[1], next[2] - start[2])

        coords = [start, next]
        for i in range(ship.size - 2):
            next = (next[0] + direction[0], next[1] + direction[1], next[2] + direction[2])
            if not self.is_in_board(next[0], next[1], next[2]):
                return False
            if next not in legalCoords:
                return False
            coords.append(next)

        for c in coords:
            self.set(c, ship.letter)

        return True

    def set_ships_random(self, list_of_ships):
        while len(list_of_ships) > 0:
            nextShip = list_of_ships[0]
            if self.set_ship_random(nextShip):
                list_of_ships.pop(0)

    def save_board_file(self, filename):
        with open(filename, 'w+') as fd:
            fd.write('{}x{}x{}{}'.format(self.rows, self.cols, self.depth, '\n\n'))

            for d in range(self.depth):
                for r in range(self.rows):
                    line = ''
                    for c in range(self.cols):
                        char = self.get(r, c, d)
                        line += char
                    fd.write('{}\n'.format(line))
                fd.write('\n')


def random_elements(elements, num):
    chosen_elements = []
    for i in range(num):
        chosen_elements.append(list(elements)[random.randint(0, len(elements)-1)])
    return chosen_elements


def random_element(elements):
    return random_elements(elements, 1)[0]


if __name__ == '__main__':
    rubber = Ship(1, 2, 'b')
    missile = Ship(2, 3, 'p')
    submarine = Ship(3, 7, 'm')
    destroyer = Ship(4, 8, 'd')
    ships = [rubber, missile, submarine, destroyer]

    player_b_ships = random_elements(ships, 5)
    player_a_ships = [Ship(s.size, s.score, s.letter.upper()) for s in player_b_ships]  # same ships
    all_ships = player_b_ships + player_a_ships

    # board size
    rows = 12
    cols = 12
    depth = 3
    number_of_boards = 2

    out_dir = 'C:\\Git\\BattleShip\\BattleshipScripts\\Boards\\3D\\Good_Generated'

    for i in range(number_of_boards):
        board = Board(rows, cols, depth)
        board.set_ships_random(all_ships.copy())
        board.save_board_file(os.path.join(out_dir, '3d_board_{}x{}x{}_{}.sboard'.format(rows, cols, depth, i)))
