#pragma once
#include <memory>
#include <windef.h>

namespace Hawk {

class EventRouter;

namespace WindowManager
{
	void Initialize(std::shared_ptr<EventRouter>& p_EventRouter);
	void Open(HINSTANCE p_hInstance, const std::string& p_Name);
	HWND GetHandle();
	bool Update();

	using WindowSizeChangedCallback_t = std::function<void(unsigned int, unsigned int, bool)>;
	void RegisterWindowSizeChanged(WindowSizeChangedCallback_t p_Callback);
}
}