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
