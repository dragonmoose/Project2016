#pragma once
#include <memory>
#include <windef.h>
#include <functional>

namespace Hawk {

class EventRouter;

namespace WindowManager
{
	void Initialize(std::shared_ptr<EventRouter>& p_EventRouter);
	void Open(HINSTANCE p_hInstance, const std::string& p_Name);
	HWND GetHandle();
	bool Update();

	using WindowManipulatedCallback_t = std::function<void(void)>;
	void RegisterWindowManipulatedCallback(WindowManipulatedCallback_t p_Callback);

	using WindowSizeChangedCallback_t = std::function<void(UINT32, UINT32, bool)>;
	void RegisterWindowSizeChanged(WindowSizeChangedCallback_t p_Callback);
}
}