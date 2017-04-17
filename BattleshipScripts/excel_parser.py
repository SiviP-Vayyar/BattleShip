import xlwt
import xlrd
import team


class Parser:
    def __init__(self):
        self.wb = xlwt.Workbook()
        self.ws = self.wb.add_sheet('Test Results')
        self.cur_row = 1

        self.ws.write(0, 0, 'Team')
        self.ws.write(0, 1, 'ID')
        self.ws.write(0, 2, 'Comments')
        self.ws.write(0, 3, 'Final Grade')

    def write_team_name(self, name):
        self.ws.write(self.cur_row, 0, name)

    def write_id(self, id):
        self.ws.write(self.cur_row, 1, id)
        self.cur_row += 1

    def write_ids(self, ids):
        for id in ids:
            self.write_id(id)

    def write_comments(self, comments):
        self.ws.write(self.cur_row, 2, comments)

    def write_final_grade(self, grade):
        self.ws.write(self.cur_row, 3, grade)

    def parse_team(self, team_to_parse, diff_results):
        self.write_team_name(team_to_parse.name)
        if not team_to_parse.is_compiled:
            diff_results += '\n' + 'DID NOT COMPILE!!!' + '\n'
        if team_to_parse.is_bonus:
            diff_results += '\n' + 'DID THE BONUS!!!' + '\n'
            team_to_parse.grade += 5
        if not team_to_parse.has_students_file:
            diff_results += '\n' + 'Did not have the students.txt file: (-3)' + '\n'
            team_to_parse.grade -= 3
        self.write_comments(diff_results)
        self.write_final_grade(str(team_to_parse.grade))
        if len(team_to_parse.members) == 0:
            self.cur_row += 1
        else:
            self.write_ids(team_to_parse.members)

    def save_report(self, sheet_path):
        self.wb.save(sheet_path)

    def add_worksheet(self, name, list_of_strings):
        new_ws = self.wb.add_sheet(name)
        for idx, line in enumerate(list_of_strings):
            new_ws.write(idx, 0, line)


class StudetsParser:

    def __init__(self, path):
        self.wb = xlrd.open_workbook(path)
        self.ws = self.wb.sheet_by_index(0)

    def get_student_ids_from_xml(self):
        return [self.ws.cell(row, 2).value for row in range(self.ws.nrows)]

    def get_student_names_from_ids(self, ids):
        return [self.ws.cell(row, 3).value.split('@')[0] for row in range(self.ws.nrows) if self.ws.cell(row, 2).value in ids]

