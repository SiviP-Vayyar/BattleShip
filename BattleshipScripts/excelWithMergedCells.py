import xlwt
import xlrd
from xlutils.copy import copy

in_path = 'C:\\Git\\BattleShip\\BattleshipScripts\\ex2_tests\\ex2_grades.xls'
out_path = 'C:\\Git\\BattleShip\\BattleshipScripts\\ex2_tests\\ex2_final_grades.xls'
ex = 'Ex2'

wb_read = xlrd.open_workbook(in_path)
ws_read = wb_read.sheet_by_index(0)
wb_write = copy(wb_read)
ws_write = wb_write.get_sheet(0)


if __name__ == '__main__':

    merge_from_row = 3
    team_name = ''

    # Styles
    style_header = xlwt.easyxf('font: bold True; alignment: horizontal center')
    style_aligned = xlwt.XFStyle()
    alignment = xlwt.Alignment()
    alignment.vert = xlwt.Alignment.VERT_CENTER
    style_aligned.alignment = alignment
    style_aligned.alignment.wrap = 1

    for row in range(3, ws_read.nrows):
        team_name = ws_read.cell(row, 0).value

        if team_name != '':
            if row != 3:
                for col in range(3, ws_read.ncols):
                    # sheet.merge(top_row, bottom_row, left_column, right_column)
                    ws_write.merge(merge_from_row, row - 1, col, col)
                merge_from_row = row

    # deal with last team
        ws_write.merge(merge_from_row, ws_read.nrows - 1, 0, 0)
    for col in range(2, ws_read.ncols):
        ws_write.merge(merge_from_row, ws_read.nrows - 1, col, col)

    wb_write.save(out_path)
