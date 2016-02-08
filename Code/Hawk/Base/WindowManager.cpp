#include "pch.h"
#include "WindowManager.h"
#include "SystemEvents.h"
#include "Debug/Assert.h"
#include "Input/InputSystem.h"
#include <memory>

namespace Hawk {

namespace WindowManager
{
	LRESULT CALLBACK WindowProc(HWND p_hWindow, UINT p_Message, WPARAM p_wParam, LPARAM p_lParam);
	const char* n_WindowName = "HawkWindowClass";
	std::unique_ptr<InputSystem> n_InputSystem;
	HWND n_hWindow = nullptr;
	std::unique_ptr<EventManager> n_EventManager;
}

void WindowManager::Initialize(std::shared_ptr<EventRouter>& p_EventRouter)
{
	n_InputSystem = std::make_unique<InputSystem>(p_EventRouter);
	n_EventManager = std::make_unique<EventManager>(p_EventRouter);
	LOG("WindowManager initialized", "window", Info);
}

void WindowManager::Open(HINSTANCE p_hInstance, const std::string& p_Name)
{
	ASSERT(p_hInstance, "No hInstance set");
	ASSERT(!p_Name.empty(), "No window name specified");

	WNDCLASSEX l_WC = { 0 };

	l_WC.cbSize = sizeof(WNDCLASSEX);
	l_WC.style = CS_HREDRAW | CS_VREDRAW;
	l_WC.lpfnWndProc = WindowProc;
	l_WC.hInstance = p_hInstance;
	l_WC.hCursor = LoadCursor(nullptr, IDC_ARROW);
	l_WC.hbrBackground = (HBRUSH)COLOR_WINDOW;
	l_WC.lpszClassName = n_WindowName;

	THROW_IF_NOT(RegisterClassEx(&l_WC), "Failed to register windows class");

	n_hWindow = CreateWindowEx(
		0,
		n_WindowName,
		p_Name.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		Config::Instance().Get("gfx.width", 640),
		Config::Instance().Get("gfx.height", 480),
		nullptr,
		nullptr,
		p_hInstance,
		nullptr
	);
	THROW_IF_NOT(n_hWindow, "Failed to create window");
	ShowWindow(n_hWindow, SW_SHOWNORMAL);
}

HWND WindowManager::GetHandle()
{
	ASSERT(n_hWindow, "Window not created yet");
	return n_hWindow;
}

bool WindowManager::Update()
{
	MSG l_Msg;
	while (PeekMessage(&l_Msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&l_Msg);
		DispatchMessage(&l_Msg);
	}
	return l_Msg.message != WM_QUIT;
}

LRESULT CALLBACK WindowManager::WindowProc(HWND p_hWindow, UINT p_Message, WPARAM p_wParam, LPARAM p_lParam)
{
	switch (p_Message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			n_InputSystem->OnKeyDown(static_cast<unsigned char>(p_wParam));
			return 0;
		case WM_KEYUP:
			n_InputSystem->OnKeyUp(static_cast<unsigned char>(p_wParam));
			return 0;
		case WM_SIZE:
			RECT l_Rect;
			GetClientRect(n_hWindow, &l_Rect);
			{
				WindowSizeChangedEvent l_Ev(l_Rect.right - l_Rect.left, l_Rect.bottom - l_Rect.top, p_wParam == SIZE_MINIMIZED);
				n_EventManager->Send(l_Ev);
			}
			return 0;
		default:
			break;
	}
	return DefWindowProc(p_hWindow, p_Message, p_wParam, p_lParam);
}
}