# CMakeLists.txt for the Clue library

set(CLUE_LIBRARY_SOURCES
	Source/BoardGraph.cpp
	Source/BoardGraph.h
	Source/Card.cpp
	Source/Card.h
	Source/Defines.h
	Source/Thread.cpp
	Source/Thread.h
	Source/Packet.cpp
	Source/Packet.h
	Source/RingBuffer.cpp
	Source/RingBuffer.h
	Source/PacketThread.cpp
	Source/PacketThread.h
	Source/Math/Vector2D.cpp
	Source/Math/Vector2D.h
	Source/Math/Box2D.cpp
	Source/Math/Box2D.h
)

source_group("Source" TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CLUE_LIBRARY_SOURCES})

add_library(ClueLibrary STATIC
	${CLUE_LIBRARY_SOURCES}
)

target_compile_definitions(ClueLibrary PRIVATE
	NOMINMAX
	_USE_MATH_DEFINES
	CLUE_LIBRARY_EXPORT
)

target_include_directories(ClueLibrary PUBLIC
	"Source"
)

target_link_libraries(ClueLibrary PUBLIC
	Ws2_32.lib
)