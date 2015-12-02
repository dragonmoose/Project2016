#pragma once
#include "Events/IEvent.h"
#include "Input/KeyCodes.h"
#include "System/DllExport.h"

namespace Hawk {

namespace InputEvents {

	struct KeyDownEvent : IEvent
	{
		explicit KeyDownEvent(KeyCode p_Code) : m_Code(p_Code) {}
		KeyCode m_Code;
	};

	struct KeyUpEvent : IEvent
	{
		explicit KeyUpEvent(KeyCode p_Code) : m_Code(p_Code) {}
		KeyCode m_Code;
	};
}
}

HAWK_DLL_EXPORT inline std::ostream& operator<<(std::ostream& os, Hawk::InputEvents::KeyDownEvent p_Event)
{
	os << static_cast<unsigned char>(p_Event.m_Code);
	return os;
}

HAWK_DLL_EXPORT inline std::ostream& operator<<(std::ostream& os, Hawk::InputEvents::KeyUpEvent p_Event)
{
	os << static_cast<unsigned char>(p_Event.m_Code);
	return os;
}