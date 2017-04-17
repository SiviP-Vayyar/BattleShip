import team
import my_utils
import os
import difflib


class Test:

    def __init__(self, tests_path):
        self.tests_path = tests_path
        self.test_results = {}  # <name : [result lines]>
        self.test_dirs = []
        self.golden_team = None

    def test_team(self, team_to_test):
        # Can't test if didn't compile
        if not team_to_test.is_compiled:
            return ''

        output_diff = ''
        for d in self.test_dirs:
            test_name = os.path.basename(d).split('_')[1]
            output_file = os.path.join(team_to_test.out_dir, '{}.txt'.format(test_name))
            arguments = ''
            if team_to_test.is_bonus:  # Bonus run with -quiet
                arguments = ' -quiet'
            os.system('{} {}{} > {}'.format(team_to_test.exe_path, d, arguments, output_file))

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

    def get_golden(self, golden_team):
        self.test_dirs = [t for t in my_utils.get_all_sub_folders(self.tests_path) if os.path.basename(t).startswith('T')]
        for d in self.test_dirs:
            test_name = os.path.basename(d).split('_')[1]
            output_file = os.path.join(self.tests_path, '{}.txt'.format(test_name))
            os.system('{} {} > {}'.format(golden_team.exe_path, d, output_file))
            with open(output_file) as fd:
                self.test_results[test_name] = fd.readlines()
