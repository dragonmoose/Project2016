#pragma once
#include <chrono>
#include <ratio>

namespace Hawk {

class Time;

class HAWK_DLL_EXPORT Duration
{
public:
	enum class Precision
	{
		Hour,
		Minute,
		Second,
		Millisecond
	};
	Duration();
	Duration(int p_iValue, Precision p_Precision);

	friend class Time;

private:
	typedef std::chrono::duration<int, std::ratio<1, 1000>> Duration_t;
	Duration_t m_InternalDuration;
};

}