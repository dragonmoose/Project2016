#pragma once
#include <string>
#include <minwinbase.h>

namespace Hawk {

struct now {};
class Time
{
public:
	Time();
	~Time();

	void SetToNow();
	static Time Now();
	std::string ToString() const;

private:
	ULARGE_INTEGER m_InternalTime;
};
}