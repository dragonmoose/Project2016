#include "pch.h"
#include "Duration.h"

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
		default:
			THROW("Invalid precision type");
	}
}

}