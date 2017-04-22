import os


def get_all_sub_folders(path):
    all_sub_folders = []
    root, dirnames, filenames = next(os.walk(path))
    for dirname in dirnames:
        all_sub_folders.append(os.path.join(root, dirname))

    return all_sub_folders


def get_all_files(folder, suffix=''):
    all_files = []
    root, dirnames, filenames = next(os.walk(folder))
    for filename in filenames:
        if filename.endswith(suffix):
            all_files.append(os.path.join(root, filename))

    return all_files


def find_student(folder, name):
    for root, dirnames, filenames in os.walk(folder):
        for file in filenames:
            if 'students.txt' in file or 'CMake' in file:
                with open(os.path.join(root, file)) as fd:
                    content = fd.read()
                    if name in content:
                        print('{}: {}'.format(name, fd.name))


def find_files(folder, names):
    for root, dirnames, filenames in os.walk(folder):
        for file in filenames:
            for name in names:
                if name in file:
                    print('{}: {}'.format(root, name))


def cmake_lists_from_all_subfolders(path):
    folders = get_all_sub_folders(path)
    cmake = ''
    for f in folders:
        cmake += 'add_subdirectory({})\n'.format(os.path.basename(f))

    with open(os.path.join(path,'CMakeLists.txt'), 'w+') as fd:
        fd.write(cmake)


if __name__ == '__main__':
    #find_student('C:\Git\BattleShip\Development\Testing\ex1', 'noamg')
    path = 'C:\Git\BattleShip\Development\Testing\ex1'
    #cmake_lists_from_all_subfolders(path)
    #find_files(path, ['stdafx'])
    pass
