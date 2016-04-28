#include "pch.h"
#include "InputSystem.h"
#include "InputEvents.h"

namespace Hawk {

InputSystem::InputSystem(std::shared_ptr<EventRouter>& p_EventRouter)
: m_EventManager(p_EventRouter)
{
}

void InputSystem::OnKeyDown(uint8 p_ucKeyCode)
{
	if (!m_KeyTable.test(p_ucKeyCode))
	{
		m_EventManager.Send(InputEvents::KeyDownEvent(static_cast<KeyCode>(p_ucKeyCode)));
		m_KeyTable.set(p_ucKeyCode, 1);
		LOG("Key pressed: " << p_ucKeyCode, "input", Trace);
	}
}

void InputSystem::OnKeyUp(uint8 p_ucKeyCode)
{
	m_EventManager.Send(InputEvents::KeyUpEvent(static_cast<KeyCode>(p_ucKeyCode)));
	m_KeyTable.set(p_ucKeyCode, 0);
	LOG("Key released: " << p_ucKeyCode, "input", Trace);
}

}