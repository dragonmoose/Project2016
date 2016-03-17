#pragma once
#include <chrono>
#include <string>

namespace Hawk {

class Duration;

class HAWK_DLL_EXPORT Time final
{
public:
	Time();
	explicit Time(const Duration& p_Duration);
	void SetToNow();

	static Time Now();
	std::string ToString() const;

	Time& operator+=(const Duration& p_rhs);
	Time& operator-=(const Duration& p_rhs);
	Duration operator-(const Time& p_rhs) const;

	bool operator==(const Time& p_rhs) const;
	bool operator!=(const Time& p_rhs) const;
	bool operator<(const Time& p_rhs) const;
	bool operator<=(const Time& p_rhs) const;
	bool operator>(const Time& p_rhs) const;
	bool operator>=(const Time& p_rhs) const;

private:
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
	TimePoint m_TimePoint;
};
}

std::ostream& operator<<(std::ostream& os, const Hawk::Time& p_Time);