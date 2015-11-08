#pragma once
#include "Macros.h"
#include <chrono>
#include <string>

namespace Hawk {

class Duration;

class HAWK_DLL_EXPORT Time
{
public:
	Time();
	explicit Time(const Duration& p_Duration);
	~Time();

	void SetToNow();
	std::string ToString() const;

	Time& operator+=(const Duration& p_rhs);
	Time& operator-=(const Duration& p_rhs);

	Time& operator=(const Time& p_rhs);
	bool operator==(const Time& p_rhs) const;
	bool operator!=(const Time& p_rhs) const;
	bool operator<(const Time& p_rhs) const;
	bool operator<=(const Time& p_rhs) const;
	bool operator>(const Time& p_rhs) const;
	bool operator>=(const Time& p_rhs) const;

private:
	typedef std::chrono::time_point<std::chrono::system_clock> TimePoint_t;
	TimePoint_t m_TimePoint;
};
}