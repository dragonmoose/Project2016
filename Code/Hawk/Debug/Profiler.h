#pragma once
#include "System/DllExport.h"
#include "System/Time.h"
#include "Util/FileUtil.h"
#include <string>
#include <sstream>

namespace Hawk {

class HAWK_DLL_EXPORT Profiler final
{
public:
	Profiler(const std::string& p_Name);
	~Profiler();

	Profiler(const Profiler&) = delete;
	Profiler& operator=(const Profiler&) = delete;

	void Start();
	void Stop();

private:
	bool m_bRunning;
	Time m_StartTime;
	std::string m_Name;
};
}

#ifdef HAWK_DEBUG
#define PROFILE()																	\
		std::ostringstream __l_Stream__;											\
		__l_Stream__ << Hawk::FileUtil::GetFilenameWithoutPath(__FILE__) << "(";	\
		std::string __l_Func__(__FUNCTION__);										\
		std::size_t __l_Pos__ = __l_Func__.find_last_of("::");						\
		if (__l_Pos__ != std::string::npos)											\
		{																			\
			__l_Func__ = __l_Func__.substr(__l_Pos__ + 1);							\
		}																			\
		__l_Stream__ << __l_Func__ << ":" << __LINE__ << ")";						\
		Hawk::Profiler __l__ProfileScope__(__l_Stream__.str());						\
		__l__ProfileScope__.Start();
#else
	#define PROFILE()
#endif
