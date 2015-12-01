#pragma once
#include <memory>

namespace Hawk {

class EventRouter;

namespace WindowManager
{
	void Initialize(std::shared_ptr<EventRouter>& p_EventRouter);
	void Open(HINSTANCE p_hInstance, const std::string& p_Name);
	bool Update();
}
}