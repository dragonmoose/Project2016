#pragma once

#pragma warning(disable:4251)	// http://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning(disable:4275)	// https://msdn.microsoft.com/en-us/library/3tdb471s.aspx

#ifndef STATIC_BUILD
	#ifdef HAWK_ENGINE
		#define HAWK_DLL_EXPORT __declspec(dllexport)
	#else
		#define HAWK_DLL_EXPORT __declspec(dllimport)
	#endif
#else
	#define HAWK_DLL_EXPORT
#endif