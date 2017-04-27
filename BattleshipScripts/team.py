import os

def get_file_ends_with(from_dir, ends_with):
    for root, dirnames, filenames in os.walk(from_dir):
        for filename in filenames:
            if filename.endswith(ends_with):
                return os.path.join(root, filename)
    return ''


class Student:

    def __init__(self, name, id):
        self.name = name
        self.id = id

    def __eq__(self, other):
        return self.id == other.id


class Team:

    def __init__(self, team_dir):
        self.name = os.path.basename(team_dir)
        self.team_dir = team_dir
        self.is_compiled = False
        self.exe_path = ''
        self.is_bonus = False
        self.students = []
        self.has_students_file = False
        self.out_dir = os.path.join(self.team_dir, 'results')
        self.test_results = {}  # <name : [result lines]>
        self.errors = []
        self.smart_score = ''
        if not os.path.isdir(self.out_dir):
            os.mkdir(self.out_dir)

        self.get_team_exe_path()
        self.get_team_bonus()
        self.get_team_members()

    def get_team_exe_path(self):
        self.exe_path = get_file_ends_with(self.team_dir, '.exe')
        self.is_compiled = (self.exe_path is not '')

    def get_team_bonus(self):
        self.is_bonus = (get_file_ends_with(self.team_dir, 'bonus.txt') is not '')

    def get_team_members(self):
        students_file = get_file_ends_with(self.team_dir, 'students.txt')
        self.has_students_file = (students_file is not '')
        if self.has_students_file:
            with open(students_file, 'r') as fd:
                lines = fd.readlines()
                for line in lines:
                    name = ""
                    s_id = ""
                    tokens = line.split(' ')
                    for token in tokens:
                        if len(token) > 1:
                            if token.strip().isdigit():
                                s_id = str(token.strip())
                            else:
                                name = token.strip()
                    self.students.append(Student(name, s_id))
