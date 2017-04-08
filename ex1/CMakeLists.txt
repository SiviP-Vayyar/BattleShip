### YOU SHOULD EDIT THE BELOW WITH YOUR DETAILS AND FILES ###

set(user1 sivanschick)
set(user2 zoharmeir1)
set(user3 noamhadad)
set(id1 "301753018")
set(id2 "303091003")
set(id3 "305462939")
set (SOURCE_FILES
	GameBoard.h
	GameBoard.cpp
	GameException.h
	main.cpp
	GameMaker.h
	GameMaker.cpp
	Player.h
	Player.cpp
	IBattleshipGameAlgo.h
)

### DO NOT CHANGE FROM HERE ON ###

message("user: ${user1}, id: ${id1}")
message("user: ${user2}, id: ${id2}")
message("user: ${user3}, id: ${id3}")

set(EXE_NAME "ex1.${user1}")
add_executable( ${EXE_NAME}
	SOURCE_FILES
)

set_property(TARGET ${EXE_NAME} PROPERTY FOLDER "ex1/${user1}")
set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "ex1.${user1})

install (TARGETS ${EXE_NAME} DESTINATION ${INSTALL_BINARY_DIR})
