#pragma once
#include "Events/EventManager.h"
#include "Events/EventRouter.h"
#include <bitset>
#include <memory>

namespace Hawk {

class InputSystem final
{
public:
	InputSystem(std::shared_ptr<EventRouter>& p_EventRouter);

	InputSystem(const InputSystem&) = delete;
	InputSystem& operator=(const InputSystem&) = delete;

	void OnKeyDown(unsigned char p_ucKeyCode);
	void OnKeyUp(unsigned char p_ucKeyCode);

private:
	EventManager m_EventManager;
	std::bitset<256> m_KeyTable;
};


}