import os
import my_utils
from shutil import move, copyfile, copy_tree


def generate_teams_folders(teams_parent_dir):
    for root, dirnames, filenames in os.walk(teams_parent_dir):
        for filename in filenames:
            if filename.endswith('.exe'):
                full_dir_name = os.path.join(teams_parent_dir, os.path.basename(filename).split('.')[0])
                if not os.path.isdir(full_dir_name):
                    os.mkdir(full_dir_name)
                move(os.path.join(root, filename), os.path.join(full_dir_name, filename))

        for filename in filenames:
            if not os.path.isdir(filename):
                full_dir_name = os.path.join(teams_parent_dir, os.path.basename(filename).split('.')[0])
                if os.path.isdir(full_dir_name):
                    move(os.path.join(root, filename), os.path.join(full_dir_name, filename))


def get_team_name_from_source_folder(source_folder):
    for root, dirnames, filenames in os.walk(source_folder):
        for filename in filenames:
            if 'CMakeLists' in filename:
                with open(os.path.join(root, filename)) as fd:
                    user1_line = [l for l in fd.readlines() if 'set' in l and 'user1' in l]
                    if len(user1_line) > 0:
                        pos_start = user1_line[0].find('user1 ')
                        pos_end = user1_line[0].find(')')
                        if pos_end > 0 and pos_start > 0:
                            return user1_line[0][pos_start + 6: pos_end].strip()
    print('Error: Could not read team name from CMakeLists: {}'.format(source_folder))
    return ''


def copy_file_from_source_to_build(team_source_dir, build_dir, filename):
    team_name = get_team_name_from_source_folder(team_source_dir)
    if team_name is not '':
        dst_team_folder = os.path.join(build_dir, team_name)
        if not os.path.isdir(dst_team_folder):
            os.mkdir(dst_team_folder)
        src_path = os.path.join(team_source_dir, filename)
        if os.path.isfile(src_path):
            dst_path = os.path.join(dst_team_folder, filename)
            copyfile(src_path, dst_path)


def copy_tests_to_team_build_folder(test_dirs, build_dir):
    team_dirs = my_utils.get_all_sub_folders(build_dir)
    for d in test_dirs:
        for t in team_dirs:
            copy_tree(d, t)

def copy_extra_files(source_dir, build_dir):
    teams_dirs = [d for d in my_utils.get_all_sub_folders(source_dir) if os.path.basename(d).startswith('ex')]
    for full_team_src_path in teams_dirs:
        copy_file_from_source_to_build(full_team_src_path, build_dir, 'students.txt')

    team_build_dirs = my_utils.get_all_sub_folders(build_dir)
    for team_build_path in team_build_dirs:
        for root, dirnames, filenames in os.walk(team_build_path):
            t0 = os.path.join(team_build_path, 'T0_Game1_MyManager')
            t1 = os.path.join(team_build_path, 'T1_Game2_Naive_File')
            t2 = os.path.join(team_build_path, 'T2_Game3_Smart_Naive')
            for filename in filenames:
                src_filename = os.path.join(team_build_path, filename)
                if filename.endswith('.file.dll'):
                    copyfile(src_filename, os.path.join(t0, filename))
                    copyfile(src_filename, os.path.join(t1, filename))
                if filename.endswith('.naive.dll'):
                    copyfile(src_filename, os.path.join(t1, filename))
                    copyfile(src_filename, os.path.join(t2, filename))
                if filename.endswith('.smart.dll'):
                    copyfile(src_filename, os.path.join(t0, filename))
                    copyfile(src_filename, os.path.join(t2, filename))
