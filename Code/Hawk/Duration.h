#pragma once
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

	Duration();
	Duration(int p_iValue, Precision p_Precision);
	std::string ToString() const;
	void SetToZero();

	bool operator==(const Duration& p_rhs) const;
	bool operator!=(const Duration& p_rhs) const;
	bool operator<(const Duration& p_rhs) const;
	bool operator<=(const Duration& p_rhs) const;
	bool operator>(const Duration& p_rhs) const;
	bool operator>=(const Duration& p_rhs) const;

	Duration& operator+=(const Duration& p_rhs);

	friend class Time;

private:
	typedef std::chrono::steady_clock::duration Duration_t;
	Duration_t m_InternalDuration;
};

}

HAWK_DLL_EXPORT std::ostream& operator<<(std::ostream& os, const Hawk::Duration& p_Duration);