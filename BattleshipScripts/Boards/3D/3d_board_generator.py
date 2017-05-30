import random

class Ship:
    def __init__(self, size, score, letter):
        self.size = size
        self.score = score
        self.letter = letter


class Board:
    def __init__(self, r, c, d):
        self._board = [[[' ' for k in range(r)] for j in range(c)] for i in range(d)]
        self.rows = r
        self.cols = c
        self.depth = d

    def is_in_board(self, r, c, d):
        return 0 < r & r <= self.rows & 0 < c & c <= self.cols & 0 < d & d <= self.depth

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
                coords += (row, col, depth)

        return coords

    def get_all_legal_coordinates(self):
        allCoordinates = {(i, j, k) for i in range(self.rows) for j in range(self.cols) for k in range(self.depth)}
        allLegalCoordinated = allCoordinates

        for coord in allCoordinates:
            if self._board != ' ':
                coordsToRemove = self.get_adjacent_coordinates(coord.r, coord.c, coord.d)
                coordsToRemove += coord
                for cRemove in coordsToRemove:
                    allLegalCoordinated.discard(cRemove)

        return allLegalCoordinated

    def set_ships_random(self, list_of_ships):
        pass  # TODO: implement

    def save_board_file(self, filename):
        pass  # TODO: implement


def random_elements(elements, num):
    chosen_elements = []
    for i in range(num):
        chosen_elements.append(elements[random.randint(0, len(elements))])
    return chosen_elements

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
    rows = 10
    cols = 10
    depth = 10
    number_of_boards = 5

    for i in range(number_of_boards):
        board = Board(rows, cols, depth)
        board.set_ships_random(all_ships)
        board.save_board_file('3d_board_{}'.format(i))
