# CMakeLists.txt for the Clue server

set(CLUE_SERVER_SOURCE
	Source/Main.cpp
	Source/Main.h
	Source/Server.cpp
	Source/Server.h
	Source/Player.cpp
	Source/Player.h
	Source/GameStates.cpp
	Source/GameStates.h
)

source_group("Source" TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CLUE_SERVER_SOURCE})

add_executable(ClueServer
	${CLUE_SERVER_SOURCE}
)

target_link_libraries(ClueServer PRIVATE
	ClueLibrary
)