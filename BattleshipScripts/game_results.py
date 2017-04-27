class GameResults:

    def __init__(self, file):
        self.file = file
        self.winner_declared = ''
        self.winner_actual = ''
        self.score_a = -1
        self.score_b = -1
        self.tie = False
        self.has_score = False
        self.right_winner = False

        self.get_game_results_from_results_file()
        self.declared_right_winner()

    def get_game_results_from_results_file(self):
        with open(self.file) as fd:
            lines = fd.readlines()
            for line in lines:
                sline = line.strip().replace(' ','').lower()
                if 'won' in sline:
                    if 'awon' in sline:
                        self.winner_declared = 'A'
                    else:
                        self.winner_declared = 'B'
                elif 'playera' in sline:
                    sa = sline.split(':')[-1]
                    if all(c.isdigit() for c in sa):
                        self.score_a = int(sa)
                elif 'playerb' in sline:
                    sb = sline.split(':')[-1]
                    if all(c.isdigit() for c in sb):
                        self.score_b = int(sb)

        if self.score_a >= 0 and self.score_b >= 0:
            if self.score_a > self.score_b:
                self.winner_actual = 'A'
                self.has_score = True
            elif self.score_a < self.score_b:
                self.winner_actual = 'B'
                self.has_score = True
            else:
                self.tie = True
                self.has_score = True

    def declared_right_winner(self):
        if not self.has_score:
            self.right_winner = False
        elif self.tie and self.winner_actual is '' and self.winner_declared is '':
            self.right_winner = True
        elif self.winner_actual == self.winner_declared:
            self.right_winner = True
        else:
            self.right_winner = False


if __name__ == '__main__':
    res = GameResults('C:/Git/BattleShip/BattleshipScripts/ex1_tests/results/Game1.txt')
    print(1)
