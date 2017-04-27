import zipfile
from my_utils import *
import os
from generate_test_results import get_team_name_from_source_folder
from distutils.dir_util import copy_tree
import shutil
from cmake_authenticator import authenticate_cmake_files

###############
EXCERSICE = 'ex2'
###############

zip_folder = 'C:/Git/BattleShip/Development/Testing/{}'.format(EXCERSICE)
zip_tmp_folder = os.path.join(zip_folder, 'tmp')
CMakeLists = '' #'"'add_subdirectory({}_sivanschick)\n'.format(EXCERSICE)
all_zips = get_all_files(zip_folder, suffix='.zip')

for zip_file in all_zips:
    if not os.path.isdir(zip_tmp_folder):
        os.mkdir(zip_tmp_folder)

    # unzip team
    zip_ref = zipfile.ZipFile(zip_file, 'r')
    zip_ref.extractall(zip_tmp_folder)
    zip_ref.close()

    # get team name and copy contents
    team_name = get_team_name_from_source_folder(zip_tmp_folder)
    if team_name is not '':
        team_dir_name = '{}_{}'.format(EXCERSICE, team_name)
        team_path = os.path.join(zip_folder, team_dir_name)
        if not os.path.isdir(team_path):
            os.mkdir(team_path)
            fromDirectory = zip_tmp_folder
            toDirectory = os.path.join(team_path)
            copy_tree(fromDirectory, toDirectory)
            CMakeLists += 'add_subdirectory({})\n'.format(team_dir_name)
        os.remove(zip_file)

    # clean tmp folder
    shutil.rmtree(zip_tmp_folder)


with open(os.path.join(zip_folder, 'CMakeLists.txt'), 'w+') as fd:
    fd.write(CMakeLists)


# Unpack folder within folder
all_subfodlers = get_all_sub_folders(zip_folder)

for f in all_subfodlers:
    in_folders = get_all_sub_folders(f)
    if len(in_folders) > 1:
        print('Error: folder {} contains subfolders'.format(f))
    elif len(in_folders) == 1:
        copy_tree(in_folders[0], f)
        shutil.rmtree(in_folders[0])


# Report all bad cmake files
print('=== Bad CMakeLists Files ===')
bad_teams = authenticate_cmake_files(zip_folder)
print('\n'.join(bad_teams))

print('=== Finished ===')
input('Press Enter To Continue...')