import team
import my_utils
import os
import difflib
from game_results import GameResults


DEBUG = True
AUTO_QUIET = True
BACKSLASH = True
BEST_OUT_OF = 3
test_only = []
ignore_tests = []

TestError = {}
###### Technical issues #######
TestError[0] = 'Late submission'
TestError[1] = 'Late submission second time'
TestError[2] = 'CMake issues'
TestError[3] = 'Submitted extra files'
TestError[4] = 'Did not compile'
TestError[5] = 'Argument support errors'
TestError[6] = 'No Students.txt file'
TestError[7] = 'General issues'
TestError[8] = 'CMake - no need for cmake_minimum_required(VERSION 3.8) - no points were taken off'
TestError[9] = 'Tabs instead of spaces in students.txt - no points were taken off'
TestError[10] = 'Wrong/No winner'
TestError[11] = 'T0 - wrong score'
TestError[12] = 'T1 - wrong score'
TestError[13] = 'T2 - Smart lost to Naive'
TestError[14] = 'T2 - Did not run with our game manager'

###### Code issues #######

TestError[15] = 'Compilation warnings'
TestError[16] = 'Did not compile - submitted late'
TestError[17] = 'Unused variable'
TestError[18] = 'Member function may be static'
TestError[19] = 'Member function may be const'
TestError[20] = ''
TestError[21] = ''
TestError[22] = ''
TestError[23] = ''
TestError[24] = ''
TestError[25] = ''
TestError[26] = ''
TestError[27] = 'Access violation on call to init(<actual_path>)'
TestError[28] = 'Runtime errors'
TestError[29] = 'Returned <-1,-1> while there are still ships on the board'
TestError[30] = 'Out of board attacks'
TestError[31] = 'Access violation reading location'
TestError[32] = ''
TestError[33] = ''
TestError[34] = ''
TestError[35] = ''
TestError[36] = ''
TestError[37] = ''
TestError[38] = ''

###### Prints issues #######

TestError[39] = 'Bad prints'
TestError[40] = 'Wrong path'
TestError[41] = 'Missing board file'
TestError[42] = 'Missing an algorithm'
TestError[43] = 'Cannot load dll'
TestError[44] = 'Algorithm initialization failed'
TestError[45] = 'Wrong size or shape'
TestError[46] = 'Too many'
TestError[47] = 'Too few'
TestError[48] = 'Adjacent'


class Test:
    def __init__(self, tests_path):
        self.tests_path = tests_path
        self.test_results = {}  # <name : [result lines]>
        self.test_dirs = []
        self.golden_team = None

    def test_team_ex1(self, team_to_test):
        # Can't test if didn't compile
        if not team_to_test.is_compiled:
            return ''

        output_diff = ''
        if len(test_only) is not 0:
            to_test = [t for t in self.test_dirs if os.path.basename(t).split('_')[0] in test_only]
        else:
            to_test = [t for t in self.test_dirs if os.path.basename(t).split('_')[0] not in ignore_tests]
        for d in to_test:
            test_name = os.path.basename(d).split('_')[1]
            output_file = os.path.join(team_to_test.out_dir, '{}.txt'.format(test_name))
            arguments = ''
            if AUTO_QUIET or team_to_test.is_bonus:  # Bonus run with -quiet
                arguments = ' -quiet'
            if BACKSLASH:
                output_file = output_file.replace("\\", "/")
                d = d.replace("\\", "/")
            execute_line = '{} {}{} > {}'.format(team_to_test.exe_path, d, arguments, output_file)
            if DEBUG:
                print(execute_line)
                print()
            os.system(execute_line)
            #subprocess.Popen(execute_line, cwd=os.path.dirname(team_to_test.exe_path))
            #subprocess.call(execute_line, shell=True)

            # Read test results
            with open(output_file) as fd:
                team_to_test.test_results[test_name] = fd.readlines()

            # Diff
            diff = difflib.ndiff(self.test_results[test_name], team_to_test.test_results[test_name])
            changes = ''.join([l for l in diff])
            changes_lines = [l for l in changes.split('\n') if not l.startswith(' ')]
            if ''.join(changes_lines) is '':  # No Changes
                continue
            else:
                output_diff += '##########' + '\n'
                output_diff += test_name + '\n'
                output_diff += '##########' + '\n\n'
                output_diff += changes + '\n'

        return output_diff

    def get_golden_ex1(self, golden_team):
        self.golden_team = golden_team
        self.test_dirs = [t for t in my_utils.get_all_sub_folders(self.tests_path) if
                          os.path.basename(t).startswith('T')]
        for d in self.test_dirs:
            test_name = os.path.basename(d).split('_')[1]
            output_file = os.path.join(self.tests_path, '{}.txt'.format(test_name))
            if BACKSLASH:
                output_file = output_file.replace("\\", "/")
                d = d.replace("\\", "/")
            os.system('{} {} > {}'.format(golden_team.exe_path, d, output_file))
            with open(output_file) as fd:
                self.test_results[test_name] = fd.readlines()

    def get_golden_ex2(self, golden_team):
        self.golden_team = golden_team
        # only tests 1 & 2 need printed results
        self.test_dirs = [t for t in my_utils.get_all_sub_folders(golden_team.team_dir) if
                          os.path.basename(t).startswith('T')]
        for d in self.test_dirs:
            test_name = os.path.basename(d)[os.path.basename(d).find('_')+1:]
            output_file = os.path.join(self.tests_path, '{}.txt'.format(test_name))
            if BACKSLASH:
                output_file = output_file.replace("\\", "/")
                d = d.replace("\\", "/")
            os.system('{} {} -quiet > {}'.format(golden_team.exe_path, d, output_file))
            with open(output_file) as fd:
                self.test_results[test_name] = fd.readlines()
        self.test_dirs = [os.path.join(os.path.dirname(os.path.dirname(d)), os.path.basename(d)) for d in self.test_dirs]

    def test_team_ex2(self, team_to_test):
        # Can't test if didn't compile
        if not team_to_test.is_compiled:
            return ''

        arguments = ''
        if AUTO_QUIET or team_to_test.is_bonus:  # Bonus run with -quiet
            arguments = ' -quiet'

        output_diff = ''
        team_test_dirs = [t for t in my_utils.get_all_sub_folders(team_to_test.team_dir) if os.path.basename(t).startswith('T')]
        if len(test_only) is not 0:
            to_test = [t for t in team_test_dirs if os.path.basename(t).split('_')[0] in test_only]
        else:
            to_test = [t for t in team_test_dirs if os.path.basename(t).split('_')[0] not in ignore_tests]

        to_test.sort()
        for d in to_test:  # first 2 tests need diff
            do_diff = False
            test_name = os.path.basename(d)[os.path.basename(d).find('_')+1:]
            output_file = os.path.join(team_to_test.out_dir, '{}.txt'.format(test_name))
            curr_output = output_file
            if BACKSLASH:
                output_file = output_file.replace("\\", "/")
                d = d.replace("\\", "/")

            if 'Game1' in test_name or 'Game2' in test_name:
                execute_line = '{} {}{} > {}'.format(team_to_test.exe_path, d, arguments, output_file)
                if DEBUG:
                    print(execute_line)
                    print()
                os.system(execute_line)

                do_diff = True
                # Read test results
                with open(curr_output) as fd:
                    team_to_test.test_results[test_name] = fd.readlines()

            else:  # Game3 - smart vs naive
                A = 0
                B = 0
                for i in range(BEST_OUT_OF):
                    curr_output = '{}.{}'.format(output_file, i)
                    execute_line = '{} {}{} > {}'.format(self.golden_team.exe_path, d, arguments, curr_output)
                    if DEBUG:
                        print(execute_line)
                        print()
                    os.system(execute_line)
                    results = GameResults(curr_output)
                    if not results.has_score:
                        # Read test results
                        with open(curr_output) as fd:
                            lines = fd.readlines()
                            if ''.join(lines) is '':
                                team_to_test.errors.append(14)
                            else:
                                do_diff = True
                                if test_name not in team_to_test.test_results:
                                    team_to_test.test_results[test_name] = lines
                                else:
                                    team_to_test.test_results[test_name].append(lines)
                    else:  # has score
                        if not results.right_winner:
                            team_to_test.errors.append(10)
                        else:  # right winner
                            if results.score_a > results.score_b:
                                A += 1
                            elif results.score_a < results.score_b:
                                B += 1
                            else:
                                A += 1
                                B += 1
                # after a few games - player B should win:
                if A > B:
                    team_to_test.errors.append(13)
                    team_to_test.smart_score = '{} - {}'.format(A, B)

            if do_diff:
                # Diff
                diff = difflib.ndiff(self.test_results[test_name], team_to_test.test_results[test_name])
                changes = ''.join([l for l in diff])
                changes_lines = [l for l in changes.split('\n') if not l.startswith(' ')]
                if ''.join(changes_lines) is '':  # No Changes
                    continue
                else:
                    output_diff += '##########' + '\n'
                    output_diff += test_name + '\n'
                    output_diff += '##########' + '\n\n'
                    output_diff += changes + '\n'

        return output_diff
