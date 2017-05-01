from generate_test_results import *
from team import Team
from test import Test
import excel_parser

if __name__ == '__main__':

    # Hard Coded Paths
    tests_path = 'C:/Git/BattleShip/BattleshipScripts/ex1_tests'
    my_dir = 'C:/Git/BattleShip/BattleshipScripts/ex1_tests'
    teams_parent_dir = "C:/Git/BattleShip/build/Windows-x64-Release/output"
    # teams_parent_dir = 'C:\\Git\\BattleShip\\BattleshipScripts'
    teams_source_dir = "C:/Git/BattleShip/Development/Testing/ex1"
    students_xlsx = "C:/Git/BattleShip/BattleshipScripts/AllStudents.xlsx"
    ignore_teams_list = []
    test_only_teams = [
                       'galklein'
                       ]

    # Generate Results Excell:
    parser = excel_parser.Parser()
    students_parser = excel_parser.StudetsParser(students_xlsx)

    # Generate Test object
    test = Test(tests_path)

    # Generate golden standard test (Me)
    team_me = Team(my_dir)
    test.get_golden_ex1(team_me)

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
        output_diff = test.test_team_ex1(team_to_test)
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
