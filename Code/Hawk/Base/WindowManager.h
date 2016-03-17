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
	HWND GetHWND();
	HINSTANCE GetHInstance();
	bool Update();

	using WindowManipulatedCallback = std::function<void(void)>;
	void RegisterWindowManipulatedCallback(WindowManipulatedCallback p_Callback);

	using WindowSizeChangedCallback = std::function<void(UINT32, UINT32, bool)>;
	void RegisterWindowSizeChanged(WindowSizeChangedCallback p_Callback);
}
}