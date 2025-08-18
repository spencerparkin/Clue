#pragma once

#if defined CLUE_LIBRARY_EXPORT
#	define CLUE_LIBRARY_API		__declspec(dllexport)
#elif defined CLUE_LIBRARY_IMPORT
#	define CLUE_LIBRARY_API		__declspec(dllimport)
#else
#	define CLUE_LIBRARY_API
#endif

#define CLUE_NUM_ROOMS			9
#define CLUE_NUM_WEAPONS		6
#define CLUE_NUM_CHARACTERS		6