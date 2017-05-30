file = "BoardFactory.txt"
rows = 10
cols = 10
boardNum = 10
with open(file) as fd:
    lines = fd.readlines()

    skip = False
    for i in range(boardNum):
        filename = 'board_{}.sboard'.format(i)
        text_to_file = ''
        for idx, line in enumerate(lines[0:rows]):
            clean_line = ""
            for c in line:
                if not skip:
                    if c is not '\t':
                        if c is not '\n':
                            clean_line += c
                            skip = True
                        else:
                            clean_line += ' '
                            skip = False
                    else:
                        clean_line += ' '
                        skip = False
                else:
                    skip = False
            text_to_file += clean_line
            if idx is not rows:
                text_to_file += '\n'

        with open(filename, "w+") as wfd:
            wfd.write(text_to_file)

        lines = lines[10:]

