# CMakeLists.txt for the Clue bot

set(CLUE_BOT_SOURCE
	Source/Main.cpp
	Source/Main.h
)

source_group("Source" TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CLUE_BOT_SOURCE})

add_executable(ClueBot
	${CLUE_BOT_SOURCE}
)

target_link_libraries(ClueBot PRIVATE
	ClueLibrary
)