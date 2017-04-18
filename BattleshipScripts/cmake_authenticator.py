import os

import my_utils


root_folder = 'C:\Git\BattleShip\Development\Testing'
cmake_str = '''### DO NOT CHANGE FROM HERE ON ###

message("user: ${user1}, id: ${id1}")
message("user: ${user2}, id: ${id2}")
message("user: ${user3}, id: ${id3}")

set(EXE_NAME "ex1.${user1}")
add_executable( ${EXE_NAME}
	${SOURCE_FILES}
)
#BUILD_BINARY_OUTPUT_DIRECTORY
set_property(TARGET ${EXE_NAME} PROPERTY FOLDER "ex1/${user1}")
set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "${user1}")'''


def authenticate_cmake_files(root_folder):
    all_team_folders = my_utils.get_all_sub_folders(root_folder)
    bad_files = []
    for d in all_team_folders:
        team_name = os.path.basename(d)
        cmake_file_path = os.path.join(d, 'CMakeLists.txt')
        if not os.path.isfile(cmake_file_path):
            bad_files.append(team_name)
            continue
        with open(cmake_file_path) as fd:
            file_string = fd.read()
            if not file_string.endswith(cmake_str):
                bad_files.append(team_name)

    return bad_files
