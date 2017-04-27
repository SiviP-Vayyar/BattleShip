import team
import my_utils
import os
import difflib
from enum import Enum


DEBUG = False
AUTO_QUIET = True
BACKSLASH = True
BEST_OUT_OF = 3
test_only = []
ignore_tests = []

TestError = {}
TestError[0] = 'Wrong path'
TestError[1] = 'Missing board file'
TestError[2] = 'Missing an algorithm'
TestError[3] = 'Cannot load dll'
TestError[4] = 'Algorithm initialization failed'
TestError[5] = 'Wrong size or shape'
TestError[6] = 'Too many'
TestError[7] = 'Too few'
TestError[8] = 'Adjacent'
##############################
TestError[9] = 'Bad prints'
TestError[10] = 'Wrong/No winner'
TestError[11] = 'T1 - wrong score'
TestError[12] = 'T2 - wrong score'
TestError[13] = 'T0 - Smart lost to Naive'
TestError[14] = 'T0 - Did not run with our game manager'
TestError[15] = ''
TestError[16] = ''
TestError[17] = ''
TestError[18] = ''
TestError[19] = ''
TestError[20] = ''
TestError[21] = ''
TestError[22] = ''


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
        # only tests 1 & 2 need printed results
        self.test_dirs = [t for t in my_utils.get_all_sub_folders(golden_team.team_dir) if
                          os.path.basename(t).startswith('T1') or os.path.basename(t).startswith('T2')]
        for d in self.test_dirs:
            test_name = os.path.basename(d)[os.path.basename(d).find('_')+1:]
            output_file = os.path.join(self.tests_path, '{}.txt'.format(test_name))
            if BACKSLASH:
                output_file = output_file.replace("\\", "/")
                d = d.replace("\\", "/")
            os.system('{} {} > {}'.format(golden_team.exe_path, d, output_file))
            with open(output_file) as fd:
                self.test_results[test_name] = fd.readlines()



    def test_team_ex2(self, team_to_test):
        # Can't test if didn't compile
        if not team_to_test.is_compiled:
            return ''

        output_diff = ''
        team_test_dirs = [t for t in my_utils.get_all_sub_folders(team_to_test.team_dir) if os.path.basename(t).startswith('T')]
        if len(test_only) is not 0:
            to_test = [t for t in team_test_dirs if os.path.basename(t).split('_')[0] in test_only]
        else:
            to_test = [t for t in team_test_dirs if os.path.basename(t).split('_')[0] not in ignore_tests]

        to_test.sort()
        for d in to_test:  # first 2 tests need diff
            test_name = os.path.basename(d).split('_')[1]
            output_file = os.path.join(team_to_test.out_dir, '{}.txt'.format(test_name))
            arguments = ''
            if AUTO_QUIET or team_to_test.is_bonus:  # Bonus run with -quiet
                arguments = ' -quiet'
            if BACKSLASH:
                output_file = output_file.replace("\\", "/")
                d = d.replace("\\", "/")

            if 'Game2' in test_name or 'Game3' in test_name:
                execute_line = '{} {}{} > {}'.format(team_to_test.exe_path, d, arguments, output_file)
                if DEBUG:
                    print(execute_line)
                    print()
                os.system(execute_line)
                # subprocess.Popen(execute_line, cwd=os.path.dirname(team_to_test.exe_path))
                # subprocess.call(execute_line, shell=True)

                # Read test results
                with open(output_file) as fd:
                    team_to_test.test_results[test_name] = fd.readlines()
            else:  # Game1 - with my game maker

                execute_line = '{} {}{} > {}'.format(self.golden_team.exe_path, d, arguments, output_file)
                if DEBUG:
                    print(execute_line)
                    print()
                os.system(execute_line)


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