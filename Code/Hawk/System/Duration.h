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
	Duration(int64 p_iValue, Precision p_Precision);
	std::string ToString(Format p_Format = Format::HourMinSecMS) const;
	void SetToZero();
	bool IsZero() const;

	int64 Get(Precision p_Precision) const;
	float GetSecondsFloat() const;

	bool operator==(const Duration& p_rhs) const;
	bool operator!=(const Duration& p_rhs) const;
	bool operator<(const Duration& p_rhs) const;
	bool operator<=(const Duration& p_rhs) const;
	bool operator>(const Duration& p_rhs) const;
	bool operator>=(const Duration& p_rhs) const;

	Duration& operator+=(const Duration& p_rhs);
	Duration operator/(uint32 p_uiDenominator) const;
	Duration operator-(const Duration& p_rhs) const;

	friend class Time;

private:
	std::string ToString_HourMinSecMS() const;
	std::string ToString_Seconds() const;
	std::string ToString_LargestUnit() const;

	using InternalDuration = std::chrono::steady_clock::duration;
	InternalDuration m_Duration;
};

}

HAWK_DLL_EXPORT std::ostream& operator<<(std::ostream& os, const Hawk::Duration& p_Duration);