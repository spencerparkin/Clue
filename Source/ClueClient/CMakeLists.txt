# CMakeLists.txt for the Clue client

set(CLUE_CLIENT_SOURCES
	Source/Application.cpp
	Source/Application.h
	Source/Frame.cpp
	Source/Frame.h
	Source/Canvas.cpp
	Source/Canvas.h
)

source_group("Source" TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CLUE_CLIENT_SOURCES})

add_executable(ClueClient WIN32
	${CLUE_CLIENT_SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/wx_dpi_aware_pmv2.manifest
)

target_compile_definitions(ClueClient PUBLIC
	__WXMSW__
	_UNICODE
)

target_compile_definitions(ClueClient PRIVATE
	WIN32_LEAN_AND_MEAN
	NOMINMAX
	_USE_MATH_DEFINES
)

target_link_libraries(ClueClient
	ClueLibrary
	wx::core
	wx::base
	wx::net
	wx::gl
)