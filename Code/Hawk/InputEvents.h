#pragma once

#include "IEvent.h"

namespace Hawk {

namespace InputEvents
{
	struct KeyDownEvent : IEvent
	{
		KeyDownEvent(unsigned char p_Code) : m_Code(p_Code) {}
		unsigned char m_Code;
	};

	struct KeyUpEvent : IEvent
	{
		KeyUpEvent(unsigned char p_Code) : m_Code(p_Code) {}
		unsigned char m_Code;
	};
}
}
