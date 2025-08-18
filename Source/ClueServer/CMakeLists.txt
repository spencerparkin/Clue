# CMakeLists.txt for the Clue server

set(CLUE_SERVER_SOURCE
	Source/Main.cpp
	Source/Main.h
)

source_group("Source" TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CLUE_SERVER_SOURCE})

add_executable(ClueServer
	${CLUE_SERVER_SOURCE}
)

target_link_libraries(ClueServer PRIVATE
	ClueLibrary
)