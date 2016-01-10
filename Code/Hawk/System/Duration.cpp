#include "pch.h"
#include "Duration.h"
#include "Debug/Assert.h"
#include <iomanip>

namespace Hawk {

Duration::Duration()
{
}

Duration::Duration(int p_iValue, Precision p_Precision)
{
	switch (p_Precision)
	{
		case Precision::Hour:
			m_Duration = std::chrono::hours(p_iValue);
			break;
		case Precision::Minute:
			m_Duration = std::chrono::minutes(p_iValue);
			break;
		case Precision::Second:
			m_Duration = std::chrono::seconds(p_iValue);
			break;
		case Precision::Millisecond:
			m_Duration = std::chrono::milliseconds(p_iValue);
			break;
		case Precision::MicroSecond:
			m_Duration = std::chrono::microseconds(p_iValue);
			break;
		case Precision::NanoSecond:
			m_Duration = std::chrono::nanoseconds(p_iValue);
			break;
		default:
			THROW("Invalid precision type given: " << static_cast<int>(p_Precision));
	}
}

std::string Duration::ToString(Duration::Format p_Format) const
{
	switch (p_Format)
	{
		case Format::HourMinSecMS:
			return ToString_HourMinSecMS();
		case Format::Seconds:
			return ToString_Seconds();
		case Format::LargestUnit:
			return ToString_LargestUnit();
	}
	ASSERT_LN("Invalid duration format: " << static_cast<int>(p_Format));
	return std::string();
}

void Duration::SetToZero()
{
	m_Duration = Duration_t::zero();
}

bool Duration::IsZero() const
{
	return m_Duration == Duration_t::zero();
}

bool Duration::operator==(const Duration& p_rhs) const
{
	return m_Duration == p_rhs.m_Duration;
}

bool Duration::operator!=(const Duration& p_rhs) const
{
	return m_Duration != p_rhs.m_Duration;
}

bool Duration::operator<(const Duration& p_rhs) const
{
	return m_Duration < p_rhs.m_Duration;
}

bool Duration::operator<=(const Duration& p_rhs) const
{
	return m_Duration <= p_rhs.m_Duration;
}

bool Duration::operator>(const Duration& p_rhs) const
{
	return m_Duration > p_rhs.m_Duration;
}

bool Duration::operator>=(const Duration& p_rhs) const
{
	return m_Duration >= p_rhs.m_Duration;
}

Duration& Duration::operator+=(const Duration& p_rhs)
{
	m_Duration += p_rhs.m_Duration;
	return *this;
}

std::string Duration::ToString_HourMinSecMS() const
{
	Duration_t l_Duration(m_Duration);

	auto l_Hours = std::chrono::duration_cast<std::chrono::hours>(l_Duration);
	l_Duration -= l_Hours;
	auto l_Minutes = std::chrono::duration_cast<std::chrono::minutes>(l_Duration);
	l_Duration -= l_Minutes;
	auto l_Seconds = std::chrono::duration_cast<std::chrono::seconds>(l_Duration);
	l_Duration -= l_Seconds;
	auto l_Milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(l_Duration);

	std::ostringstream l_Stream;
	l_Stream << std::setfill('0') << std::setw(2) << l_Hours.count() << ":" <<
		std::setfill('0') << std::setw(2) << l_Minutes.count() << ":" <<
		std::setfill('0') << std::setw(2) << l_Seconds.count() << "." <<
		std::setfill('0') << std::setw(3) << l_Milliseconds.count();
	return l_Stream.str();
}

std::string Duration::ToString_Seconds() const
{
	std::chrono::duration<float> l_Duration(m_Duration);
	return std::to_string(l_Duration.count());
}

std::string Duration::ToString_LargestUnit() const
{
	auto l_Hours = std::chrono::duration_cast<std::chrono::hours>(m_Duration);
	if (l_Hours.count() > 0)
	{
		return std::to_string(l_Hours.count()) + "h";
	}
	auto l_Minutes = std::chrono::duration_cast<std::chrono::minutes>(m_Duration);
	if (l_Minutes.count() > 0)
	{
		return std::to_string(l_Minutes.count()) + "m";
	}
	auto l_Seconds = std::chrono::duration_cast<std::chrono::seconds>(m_Duration);
	if (l_Seconds.count() > 0)
	{
		return std::to_string(l_Seconds.count()) + "s";
	}
	auto l_Milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(m_Duration);
	if (l_Milliseconds.count() > 0)
	{
		return std::to_string(l_Milliseconds.count()) + "ms";
	}
	auto l_MicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(m_Duration);
	if (l_MicroSeconds.count() > 0)
	{
		return std::to_string(l_MicroSeconds.count()) + "us";
	}
	auto l_NanoSeconds = std::chrono::duration_cast<std::chrono::nanoseconds>(m_Duration);
	if (l_NanoSeconds.count() > 0)
	{
		return std::to_string(l_NanoSeconds.count()) + "ns";
	}
	return "N/A";
}
}

std::ostream& operator<<(std::ostream& os, const Hawk::Duration& p_Duration)
{
	os << p_Duration.ToString();
	return os;
}