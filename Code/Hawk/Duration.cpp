#include "pch.h"
#include "Duration.h"
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
			m_InternalDuration = std::chrono::hours(p_iValue);
			break;
		case Precision::Minute:
			m_InternalDuration = std::chrono::minutes(p_iValue);
			break;
		case Precision::Second:
			m_InternalDuration = std::chrono::seconds(p_iValue);
			break;
		case Precision::Millisecond:
			m_InternalDuration = std::chrono::milliseconds(p_iValue);
			break;
		case Precision::MicroSecond:
			m_InternalDuration = std::chrono::microseconds(p_iValue);
			break;
		case Precision::NanoSecond:
			m_InternalDuration = std::chrono::nanoseconds(p_iValue);
			break;
		default:
			THROW("Invalid precision type given: " << static_cast<int>(p_Precision));
	}
}

std::string Duration::ToString() const
{
	Duration_t l_Duration(m_InternalDuration);

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

bool Duration::operator==(const Duration& p_rhs) const
{
	return m_InternalDuration == p_rhs.m_InternalDuration;
}

}

std::ostream& operator<<(std::ostream& os, const Hawk::Duration& p_Duration)
{
	os << p_Duration.ToString();
	return os;
}