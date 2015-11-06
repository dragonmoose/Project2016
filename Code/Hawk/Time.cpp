#include "pch.h"
#include "Time.h"

namespace Hawk {

Time::Time()
{
	m_InternalTime = {};
}

Time::~Time()
{
}

void Time::SetToNow()
{
	SYSTEMTIME l_SysTime;
	FILETIME l_FileTime;
	GetSystemTime(&l_SysTime);
	SystemTimeToFileTime(&l_SysTime, &l_FileTime);
	m_InternalTime.LowPart = l_FileTime.dwLowDateTime;
	m_InternalTime.HighPart = l_FileTime.dwHighDateTime;
}

Time Time::Now()
{
	Time l_Time;
	l_Time.SetToNow();
	return l_Time;
}

std::string Time::ToString() const
{
	return std::to_string(m_InternalTime.QuadPart);
}
}