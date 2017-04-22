import os
import my_utils
from shutil import move, copyfile
from team import Team
from test import Test
import excel_parser


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


def copy_extra_files(source_dir, build_dir):
    teams_dirs = [d for d in my_utils.get_all_sub_folders(source_dir) if os.path.basename(d).startswith('ex')]
    for full_team_src_path in teams_dirs:
        # full_team_src_path = os.path.join(source_dir, os.path.basename(dir_name))
        copy_file_from_source_to_build(full_team_src_path, build_dir, 'students.txt')
        copy_file_from_source_to_build(full_team_src_path, build_dir, 'bonus.txt')


if __name__ == '__main__':

    # Hard Coded Paths
    tests_path = 'C:/Git/BattleShip/BattleshipScripts/ex1_tests'
    my_dir = 'C:/Git/BattleShip/BattleshipScripts/ex1_tests'
    teams_parent_dir = "C:/Git/BattleShip/build/Windows-x64-Release/output"
    teams_source_dir = "C:/Git/BattleShip/Development/Testing/ex1"
    students_xlsx = "C:/Git/BattleShip/BattleshipScripts/AllStudents.xlsx"
    ignore_teams_list = []
    test_only_teams = []

    # Generate Results Excell:
    parser = excel_parser.Parser()
    students_parser = excel_parser.StudetsParser(students_xlsx)

    # Generate Test object
    test = Test(tests_path)

    # Generate golden standard test (Me)
    team_me = Team(my_dir)
    test.get_golden(team_me)

    # Generate team's folders from flat binary folder
    generate_teams_folders(teams_parent_dir)

    # Copy "students.txt","bonus.txt"
    copy_extra_files(teams_source_dir, teams_parent_dir)

    # Generate Teams:
    team_dirs = [x[0] for x in os.walk(teams_parent_dir) if 'results' not in x[0]][1:]
    teams = [Team(d) for d in team_dirs]

    # Generate teams' test result:
    students_with_grades = []
    error_parsing_diff = []

    # figure out which teams to test:
    if len(test_only_teams) is not 0:
        teams = [t for t in teams if t.name in test_only_teams]
    else:
        teams = [t for t in teams if t.name not in ignore_teams_list]

    for idx, team_to_test in enumerate(teams):
        print('Started team: {}\t\t{}/{}'.format(team_to_test.name, idx + 1, len(teams)))
        output_diff = test.test_team(team_to_test)
        if len(output_diff) > 30000:
            output_diff = 'PROGRAM OUTPUT TOO LONG!!!'
            error_parsing_diff.append(team_to_test.name)
        parser.parse_team(team_to_test, output_diff)
        students_with_grades.extend([s for s in team_to_test.students])


    # Lists of special cases for the end:
    team_students_file_missing_list = [t.name for t in teams if not t.has_students_file]
    parser.add_worksheet("Missing Students File", team_students_file_missing_list)
    team_not_compiled_list = [t.name for t in teams if not t.is_compiled]
    parser.add_worksheet("Did Not Compile", team_not_compiled_list)
    is_bonus_teams_list = [t.name for t in teams if t.is_bonus]
    parser.add_worksheet("Did The Bonus", is_bonus_teams_list)
    missing_students = [s for s in students_parser.get_students_from_excel() if s not in students_with_grades]
    parser.add_missing_students(missing_students)
    parser.add_worksheet("Output Too Long", error_parsing_diff)

    # Make report of results:
    parser.save_report(os.path.join(my_dir, 'ex1.xls'))

    print('=== Finished ===')
    input('Press Enter To Continue...')
