#pragma once

#ifndef STATIC_BUILD
	#ifdef HAWK_ENGINE
		#define HAWK_DLL_EXPORT __declspec(dllexport)
	#else
		#define HAWK_DLL_EXPORT __declspec(dllimport)
	#endif
#else
	#define HAWK_DLL_EXPORT
#endif