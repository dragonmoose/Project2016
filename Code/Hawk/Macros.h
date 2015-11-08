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
	#define LOG_INFO(msg)				Hawk::LogSystem::Write(##msg, Hawk::LogSystem::Type::Info);
	#define LOG_WARNING(msg)			Hawk::LogSystem::Write(##msg, Hawk::LogSystem::Type::Warning);
	#define LOG_ERROR(msg)				Hawk::LogSystem::Write(##msg, Hawk::LogSystem::Type::Error);
	#define LOG_EXCEPTION(e)			Hawk::LogSystem::Write(e.what(), Hawk::LogSystem::Type::Exception);
#else
	#define LOG_INFO(msg)
	#define LOG_WARNING(msg)
	#define LOG_ERROR(msg)
#endif

#include "Exception.h"
#define THROW(msg)						{ std::string l_Msg(msg); throw Exception(l_Msg); }