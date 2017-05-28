import xlwt
import xlrd

in_path = 'C:\\Git\\BattleShip\\BattleshipScripts\\ex2_tests\\ex2_grades.xls'
out_path = 'C:\\Git\\BattleShip\\BattleshipScripts\\ex2_tests\\ex2_final_grades.xls'
ex = 'Ex2'

ws_read = xlrd.open_workbook(in_path).sheet_by_index(0)
wb_write = xlwt.Workbook()
ws_write = wb_write.add_sheet('{} Results'.format(ex))

def get_comments_from_row(ws_read, r):
    all_comments = ''
    for c in range(6, ws_read.ncols):
        cell_value = ws_read.cell(r, c).value
        if cell_value != '':
            all_comments += ws_read.cell(2, c).value
            if cell_value == 'X':
                all_comments += '\n'
            else:
                all_comments += ' - {}\n'.format(cell_value)

    return all_comments

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

    for col in range(1, 6):
        ws_write.write(2, col, ws_read.cell(2, col).value, style_header)

    for row in range(3, ws_read.nrows):
        team_name = ws_read.cell(row, 0).value
        student_id = ws_read.cell(row, 1).value
        ws_write.write(row, 1, student_id)

        if team_name != '':
            for col in [3, 4, 5]:
                ws_write.write(row, col, ws_read.cell(row, col).value)

            comments = get_comments_from_row(ws_read, row)
            ws_write.write(row, 2, comments, style_aligned)

            if row != 3:
                # sheet.merge(top_row, bottom_row, left_column, right_column)
                for col in [0, 2, 3, 4, 5]:
                    ws_write.merge(merge_from_row, row - 1, col, col)
                merge_from_row = row
    for col in [0, 2, 3, 4, 5]:
        ws_write.merge(merge_from_row, ws_read.nrows - 1, col, col)

    wb_write.save(out_path)
