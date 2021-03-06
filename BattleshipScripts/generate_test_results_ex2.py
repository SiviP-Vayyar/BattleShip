from generate_test_results import *
from team import Team
from test import Test
import excel_parser

if __name__ == '__main__':

    # Hard Coded Paths
    tests_path = 'C:/Git/BattleShip/BattleshipScripts/ex2_tests'
    my_dir = 'C:/Git/BattleShip/BattleshipScripts/ex2_tests/golden'
    teams_parent_dir = "C:/Git/BattleShip/build/Windows-x64-Release/output"
    teams_source_dir = "C:/Git/BattleShip/Development/Testing/ex2"
    students_xlsx = "C:/Git/BattleShip/BattleshipScripts/AllStudents.xlsx"
    ignore_teams_list = ['nofarhiki']
    test_only_teams = ['shukrun']

    # Generate Results Excell:
    parser = excel_parser.Parser()
    students_parser = excel_parser.StudetsParser(students_xlsx)

    # Generate Test object
    test = Test(tests_path)

    # Generate golden standard test (Me)
    team_me = Team(my_dir)
    test.get_golden_ex2(team_me)  # TODO: implement

    # Generate team's folders from flat binary folder
    generate_teams_folders(teams_parent_dir)

    # Copy all tests to each team's build folder
    copy_tests_to_team_build_folder(test.test_dirs, teams_parent_dir)

    # Copy "students.txt" + .dll's to test folders
    copy_extra_files(teams_source_dir, teams_parent_dir)

    # Generate Teams:
    team_dirs = [x for x in my_utils.get_all_sub_folders(teams_parent_dir) if 'results' not in x]
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
        output_diff = test.test_team_ex2(team_to_test)
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
    missing_students = [s for s in students_parser.get_students_from_excel() if s not in students_with_grades]
    parser.add_missing_students(missing_students)
    parser.add_worksheet("Output Too Long", error_parsing_diff)

    # Make report of results:
    parser.save_report(os.path.join(tests_path, 'ex2.xls'))

    print('=== Finished ===')
