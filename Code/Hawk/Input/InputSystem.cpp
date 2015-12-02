#include "pch.h"
#include "Input/InputSystem.h"
#include "Input/InputEvents.h"

namespace Hawk {

InputSystem::InputSystem(std::shared_ptr<EventRouter>& p_EventRouter)
: m_EventManager(p_EventRouter)
{
}

void InputSystem::OnKeyDown(unsigned char p_ucKeyCode)
{
	if (!m_KeyTable.test(p_ucKeyCode))
	{
		m_EventManager.Send<InputEvents::KeyDownEvent>(InputEvents::KeyDownEvent(static_cast<KeyCode>(p_ucKeyCode)));
		m_KeyTable.set(p_ucKeyCode, 1);
	}
}

void InputSystem::OnKeyUp(unsigned char p_ucKeyCode)
{
	m_EventManager.Send<InputEvents::KeyUpEvent>(InputEvents::KeyUpEvent(static_cast<KeyCode>(p_ucKeyCode)));
	m_KeyTable.set(p_ucKeyCode, 0);
}

}