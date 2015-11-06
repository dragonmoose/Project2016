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

#include "LogSystem.h"
#ifdef HAWK_DEV
	#define LOG_INFO(msg)		Hawk::LogSystem::Write(##msg, Hawk::LogSystem::Level::Info)
	#define LOG_WARNING(msg)	Hawk::LogSystem::Write(##msg, Hawk::LogSystem::Level::Warning)
	#define LOG_ERROR(msg)		Hawk::LogSystem::Write(##msg, Hawk::LogSystem::Level::Error)
#else
	#define LOG_INFO(msg)
	#define LOG_WARNING(msg)
	#define LOG_ERROR(msg)
#endif