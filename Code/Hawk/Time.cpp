#include "pch.h"
#include "Time.h"
#include "Duration.h"
#include <iomanip>
#include <sstream>

namespace Hawk {

Time::Time()
{
	SetToNow();
}

Time::Time(const Duration& p_Duration)
{
	SetToNow();
	*this += p_Duration;
}

Time::~Time()
{
}

void Time::SetToNow()
{
	m_TimePoint = std::chrono::system_clock::now();
}

Time& Time::operator+=(const Duration& p_rhs)
{
	m_TimePoint += p_rhs.m_InternalDuration;
	return *this;
}

Time& Time::operator-=(const Duration& p_rhs)
{
	m_TimePoint -= p_rhs.m_InternalDuration;
	return *this;
}

std::string Time::ToString(TimeFormat p_Format) const
{
	std::stringstream l_Stream;
	std::time_t l_Time = std::chrono::system_clock::to_time_t(m_TimePoint);
	l_Stream << std::put_time(std::localtime(&l_Time), (p_Format == TimeFormat::DateAndTime ? "%Y-%m-%d %X" : "%X"));
	return l_Stream.str();
}

Time& Time::operator=(const Time& p_rhs)
{
	m_TimePoint = p_rhs.m_TimePoint;
	return *this;
}

bool Time::operator==(const Time& p_rhs) const
{
	return m_TimePoint == p_rhs.m_TimePoint;
}

bool Time::operator!=(const Time& p_rhs) const
{
	return m_TimePoint != p_rhs.m_TimePoint;
}

bool Time::operator<(const Time& p_rhs) const
{
	return m_TimePoint < p_rhs.m_TimePoint;
}

bool Time::operator<=(const Time& p_rhs) const
{
	return m_TimePoint <= p_rhs.m_TimePoint;
}

bool Time::operator>(const Time& p_rhs) const
{
	return m_TimePoint > p_rhs.m_TimePoint;
}
bool Time::operator>=(const Time& p_rhs) const
{
	return m_TimePoint >= p_rhs.m_TimePoint;
}

}