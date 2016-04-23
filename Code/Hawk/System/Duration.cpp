#include "pch.h"
#include "Duration.h"
#include <iomanip>

namespace Hawk {

const double Duration::sc_dSecToNano = 1000000000;
const double Duration::sc_dNanoToSec = 1.0 / sc_dSecToNano;

Duration::Duration()
{
	SetToZero();
}

Duration::Duration(int64 p_iValue, Precision p_Precision)
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
			ASSERT_LN("Invalid precision type given: " << static_cast<int32>(p_Precision));
	}
}

Duration::Duration(float p_fSeconds)
{
	m_Duration = std::chrono::nanoseconds(static_cast<std::chrono::nanoseconds::rep>(p_fSeconds * sc_dSecToNano));
}


std::string Duration::ToString(Format p_Format) const
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
	ASSERT_LN("Invalid duration format: " << static_cast<int32>(p_Format));
	return std::string();
}

void Duration::SetToZero()
{
	m_Duration = InternalDuration::zero();
}

bool Duration::IsZero() const
{
	return m_Duration == InternalDuration::zero();
}

int64 Duration::Get(Precision p_Precision) const
{
	switch (p_Precision)
	{
		case Precision::Hour:
			return std::chrono::duration_cast<std::chrono::hours>(m_Duration).count();
		case Precision::Minute:
			return std::chrono::duration_cast<std::chrono::minutes>(m_Duration).count();
		case Precision::Second:
			return std::chrono::duration_cast<std::chrono::seconds>(m_Duration).count();
		case Precision::Millisecond:
			return std::chrono::duration_cast<std::chrono::milliseconds>(m_Duration).count();
		case Precision::MicroSecond:
			return std::chrono::duration_cast<std::chrono::microseconds>(m_Duration).count();
		case Precision::NanoSecond:
			return std::chrono::duration_cast<std::chrono::nanoseconds>(m_Duration).count();
	}
	ASSERT_LN("Invalid precision type given: " << static_cast<int>(p_Precision));
	return 0;
}

float Duration::GetSecondsFloat() const
{
	return static_cast<float>(sc_dNanoToSec * std::chrono::duration_cast<std::chrono::nanoseconds>(m_Duration).count());
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

Duration& Duration::operator-=(const Duration& p_rhs)
{
	m_Duration -= p_rhs.m_Duration;
	return *this;
}


Duration Duration::operator/(uint32 p_uiDenominator) const
{
	return Duration((m_Duration / p_uiDenominator).count(), Precision::NanoSecond);
}

Duration Duration::operator-(const Duration& p_rhs) const
{
	return Duration((m_Duration - p_rhs.m_Duration).count(), Precision::NanoSecond);
}

std::string Duration::ToString_HourMinSecMS() const
{
	InternalDuration l_Duration(m_Duration);

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