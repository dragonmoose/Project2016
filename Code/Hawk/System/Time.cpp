#include "pch.h"
#include "Time.h"
#include "Duration.h"
#include <iomanip>

namespace Hawk {

namespace
{
	Time n_ZeroTimeRef = Time::Now();
}

Time::Time()
{
}

Time::Time(const Duration& p_Duration)
{
	SetToNow();
	*this += p_Duration;
}

void Time::SetToNow()
{
	m_TimePoint = std::chrono::steady_clock::now();
}

Time Time::Now()
{
	Time l_Time;
	l_Time.SetToNow();
	return l_Time;
}

std::string Time::ToString() const
{
	return (*this - n_ZeroTimeRef).ToString();
}

Time& Time::operator+=(const Duration& p_rhs)
{
	m_TimePoint += p_rhs.m_Duration;
	return *this;
}

Time& Time::operator-=(const Duration& p_rhs)
{
	m_TimePoint -= p_rhs.m_Duration;
	return *this;
}

Duration Time::operator-(const Time& p_rhs) const
{
	Duration l_Duration;
	l_Duration.m_Duration = m_TimePoint - p_rhs.m_TimePoint;
	return l_Duration;
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

std::ostream& operator<<(std::ostream& os, const Hawk::Time& p_Time)
{
	os << p_Time.ToString();
	return os;
}