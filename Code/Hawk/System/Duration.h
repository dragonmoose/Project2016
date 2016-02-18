#pragma once
#include "System/Types.h"
#include <chrono>

namespace Hawk {

class Time;

class HAWK_DLL_EXPORT Duration final
{
public:
	enum class Precision
	{
		Hour,
		Minute,
		Second,
		Millisecond,
		MicroSecond,
		NanoSecond
	};

	enum class Format
	{
		HourMinSecMS,
		Seconds,
		LargestUnit
	};

	Duration();
	Duration(INT32 p_iValue, Precision p_Precision);
	std::string ToString(Format p_Format = Format::HourMinSecMS) const;
	void SetToZero();
	bool IsZero() const;

	INT64 Get(Precision p_Precision) const;

	bool operator==(const Duration& p_rhs) const;
	bool operator!=(const Duration& p_rhs) const;
	bool operator<(const Duration& p_rhs) const;
	bool operator<=(const Duration& p_rhs) const;
	bool operator>(const Duration& p_rhs) const;
	bool operator>=(const Duration& p_rhs) const;

	Duration& operator+=(const Duration& p_rhs);
	Duration operator/(UINT32 p_iDenominator) const;
	Duration operator-(const Duration& p_rhs) const;

	friend class Time;

private:
	std::string ToString_HourMinSecMS() const;
	std::string ToString_Seconds() const;
	std::string ToString_LargestUnit() const;

	using Duration_t = std::chrono::steady_clock::duration;
	Duration_t m_Duration;
};

}

HAWK_DLL_EXPORT std::ostream& operator<<(std::ostream& os, const Hawk::Duration& p_Duration);