#pragma once

#pragma warning(disable:4251)	// http://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning(disable:4275)	// https://msdn.microsoft.com/en-us/library/3tdb471s.aspx

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
	#define NOMINMAX
#endif
#include <windows.h>
#include <iostream>

#pragma warning (push)
#pragma warning (disable:4201) // nonstandard extension used: nameless struct/union
#include <glm/glm.hpp>
#pragma warning (pop)