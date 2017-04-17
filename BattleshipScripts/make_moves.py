import itertools
from random import shuffle, randint

rows = 10
cols = 10

def save_list_of_moves_to_file(list, filename, clean = True):
    list_of_move_strings = ['{}, {}'.format(r, c) for r, c in list]

    #Save a clean copy:
    with open('clean_{}'.format(filename), "w+") as wfd:
        wfd.writelines('\n'.join(list_of_move_strings))

    if not clean:
        how_dirty = 20

        for i in range(how_dirty):
            #dirty comma spaces
            idx = randint(0, len(list_of_move_strings))
            before = randint(1, how_dirty//2)
            after = randint(1, how_dirty//2)
            dirty_string = list_of_move_strings[idx].replace(',', ' '*before + ',').replace(',', ',' + ' '*after)
            list_of_move_strings[idx] = dirty_string

        #Save a dirty but legal copy:
        with open('dirty_but_legal_{}'.format(filename), "w+") as wfd:
            wfd.writelines('\n'.join(list_of_move_strings))

        for i in range(how_dirty):
            # big row
            idx = randint(0, len(list_of_move_strings))
            r = randint(rows + 1, rows + 10)
            c = randint(1, cols)
            list_of_move_strings.insert(idx, '{}, {}'.format(r, c))
            # big col
            idx = randint(0, len(list_of_move_strings))
            r = randint(1, rows)
            c = randint(cols + 1, cols + 10)
            list_of_move_strings.insert(idx, '{}, {}'.format(r, c))
            # no comma
            idx = randint(0, len(list_of_move_strings))
            r = randint(1, rows)
            c = randint(1, cols)
            list_of_move_strings.insert(idx, '{} {}'.format(r, c))
            # junk
            junk = ["Moishe", "shpritz", "banana", "", " "]
            idx = randint(0, len(list_of_move_strings))
            list_of_move_strings.insert(idx, junk[idx % 5])

        # Save a dirty and ilegal copy:
        with open('dirty_ilegal_{}'.format(filename), "w+") as wfd:
            wfd.writelines('\n'.join(list_of_move_strings))


if __name__ == '__main__':
    list_of_moves = []
    for i in itertools.product([i+1 for i in range(10)],[i+1 for i in range(10)]):
        list_of_moves.append(i)

    shuffle(list_of_moves)
    save_list_of_moves_to_file(list_of_moves, 'movesA.attack-a', False)
    shuffle(list_of_moves)
    save_list_of_moves_to_file(list_of_moves, 'movesB.attack-b', False)
