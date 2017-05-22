import xlwt
import xlrd
import team
from team import Student
from test import TestError

ERRORS_START_COL = 6
ERROR_MARK = 'X'

class Parser:
    def __init__(self):
        self.wb = xlwt.Workbook()
        self.ws = self.wb.add_sheet('Test Results')
        self.ws.panes_frozen = True
        self.ws.remove_splits = True
        self.ws.vert_split_pos = 1
        self.ws.horz_split_pos = 1
        self.cur_row = 1

        # styles
        self.style_header = xlwt.easyxf('font: bold True; alignment: horizontal center')
        self.style_center = xlwt.easyxf('alignment: horizontal center')
        self.ws.row(0).set_style(self.style_header)
        self.ws.col(5).set_style(self.style_header)

        self.ws.write(0, 0, 'Team', self.style_header)
        self.ws.write(0, 1, 'ID', self.style_header)
        self.ws.write(0, 2, 'Comments', self.style_header)
        self.ws.write(0, 3, 'Grade', self.style_header)
        self.ws.write(0, 4, 'Bonus', self.style_header)
        self.ws.write(0, 5, 'Final Grade', self.style_header)

        #errors header
        for num, val in TestError.items():
            self.ws.write(0, ERRORS_START_COL+num, val, self.style_header)


    def write_team_name(self, name):
        self.ws.write(self.cur_row, 0, name)

    def write_id(self, id):
        self.ws.write(self.cur_row, 1, id)
        self.cur_row += 1

    def write_ids(self, students):
        for s in students:
            self.write_id(s.id)

    def write_comments(self, comments):
        self.ws.write(self.cur_row, 2, comments)

    def parse_team(self, team_to_parse, diff_results):
        self.write_team_name(team_to_parse.name)
        if not team_to_parse.is_compiled:
            diff_results += '\n' + 'DID NOT COMPILE!!!' + '\n'
        if team_to_parse.is_bonus:
            diff_results += '\n' + 'DID THE BONUS!!!' + '\n'
        self.write_comments(diff_results)
        self.write_errors(team_to_parse.errors)

        if len(team_to_parse.students) == 0:
            self.cur_row += 1
        else:
            self.write_ids(team_to_parse.students)

    def write_errors(self, errors):
        for num in set(errors):
            self.ws.write(self.cur_row, ERRORS_START_COL+num, ERROR_MARK, self.style_center)

    def add_missing_students(self, students_list):
        for s in students_list:
            self.write_team_name(s.name)
            self.write_id(s.id)

    def save_report(self, sheet_path):
        # center horizontal all columns
        for i in range(1, 200):
            self.ws.col(i).set_style(self.style_center)
        self.wb.save(sheet_path)

    def add_worksheet(self, name, list_of_strings):
        new_ws = self.wb.add_sheet(name)
        for idx, line in enumerate(list_of_strings):
            new_ws.write(idx, 0, line)


class StudetsParser:

    def __init__(self, path):
        self.wb = xlrd.open_workbook(path)
        self.ws = self.wb.sheet_by_index(0)

    def get_students_from_excel(self):
        return [Student(self.ws.cell(row, 3).value.split('@')[0], self.ws.cell(row, 2).value) for row in range(self.ws.nrows)]