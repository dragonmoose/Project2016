#include "pch.h"
#include "WindowManager.h"
#include "SystemEvents.h"
#include "Input/InputSystem.h"
#include <memory>

namespace Hawk {

namespace WindowManager
{
	LRESULT CALLBACK WindowProc(HWND p_hWindow, UINT p_Message, WPARAM p_wParam, LPARAM p_lParam);
	void CallWindowSizeChanged(bool p_bMinimized);
	void CallWindowManipulated();

	const char* n_WindowName = "HawkWindowClass";
	std::unique_ptr<InputSystem> n_InputSystem;
	HWND n_hWindow = nullptr;
	HINSTANCE n_hInstance = nullptr;
	std::unique_ptr<EventManager> n_EventManager;
	bool n_bWindowManipulated = false;

	WindowSizeChangedCallback n_WindowSizeChangedCallback;
	WindowManipulatedCallback n_WindowManipulatedCallback;
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

	n_hInstance = p_hInstance;

	WNDCLASSEX l_WC = { 0 };

	l_WC.cbSize = sizeof(WNDCLASSEX);
	l_WC.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	l_WC.lpfnWndProc = WindowProc;
	l_WC.hInstance = p_hInstance;
	l_WC.hCursor = LoadCursor(nullptr, IDC_ARROW);
	l_WC.hbrBackground = (HBRUSH)COLOR_WINDOW;
	l_WC.lpszClassName = n_WindowName;

	THROW_IF_NOT(RegisterClassEx(&l_WC), "Failed to register windows class");

	DWORD l_dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	DWORD l_dwExStyle = WS_EX_APPWINDOW;
	RECT l_Rect = { 0, 0, Config::Instance().Get("gfx.width", 640), Config::Instance().Get("gfx.height", 480) };
	if (Config::Instance().Get("gfx.fullscreen", false))
	{
		l_dwStyle |= WS_POPUP;
		DEVMODE l_DevMode = {};
		l_DevMode.dmSize = sizeof(DEVMODE);
		l_DevMode.dmPelsWidth = l_Rect.right - l_Rect.left;
		l_DevMode.dmPelsHeight = l_Rect.bottom - l_Rect.top;
		l_DevMode.dmBitsPerPel = 32;		// TODO: Should check buffer config
		l_DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		THROW_IF_NOT(ChangeDisplaySettings(&l_DevMode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL, "Failed to set fullscreen");
	}
	else
	{
		l_dwStyle |= WS_OVERLAPPEDWINDOW;
		l_dwExStyle |= WS_EX_WINDOWEDGE;
	}
	AdjustWindowRectEx(&l_Rect, l_dwStyle, FALSE, l_dwExStyle);

	n_hWindow = CreateWindowEx(
		l_dwExStyle,
		n_WindowName,
		p_Name.c_str(),
		l_dwStyle,
		0,
		0,
		l_Rect.right - l_Rect.left,
		l_Rect.bottom - l_Rect.top,
		nullptr,
		nullptr,
		p_hInstance,
		nullptr
	);
	THROW_IF_NOT(n_hWindow, "Failed to create window");
	ShowWindow(n_hWindow, SW_SHOWNORMAL);
}

HWND WindowManager::GetHWND()
{
	ASSERT(n_hWindow, "Window not created yet");
	return n_hWindow;
}

HINSTANCE WindowManager::GetHInstance()
{
	return n_hInstance;
}

bool WindowManager::Update()
{
	MSG l_Msg;
	bool l_bQuit = false;
	while (PeekMessage(&l_Msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&l_Msg);
		DispatchMessage(&l_Msg);
		if (l_Msg.message == WM_QUIT)	// WM_QUIT won't be dispatched by DispatchMessage, so need to handle it separately
		{
			l_bQuit = true;
		}
	}
	return !l_bQuit;
}

void WindowManager::RegisterWindowSizeChanged(WindowSizeChangedCallback p_Callback)
{
	n_WindowSizeChangedCallback = p_Callback;
}

void WindowManager::RegisterWindowManipulatedCallback(WindowManipulatedCallback p_Callback)
{
	n_WindowManipulatedCallback = p_Callback;
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
			if (!n_bWindowManipulated)
			{
				CallWindowSizeChanged(p_wParam == SIZE_MINIMIZED);
			}
			return 0;
		case WM_ENTERSIZEMOVE:
			CallWindowManipulated();
			n_bWindowManipulated = true;
			return 0;
		case WM_EXITSIZEMOVE:
			CallWindowSizeChanged(p_wParam == SIZE_MINIMIZED);
			n_bWindowManipulated = false;
			return 0;
		default:
			break;
	}
	return DefWindowProc(p_hWindow, p_Message, p_wParam, p_lParam);
}

void WindowManager::CallWindowSizeChanged(bool p_bMinimized)
{
	if (n_WindowSizeChangedCallback)
	{
		RECT l_Rect;
		GetClientRect(n_hWindow, &l_Rect);
		n_WindowSizeChangedCallback(l_Rect.right - l_Rect.left, l_Rect.bottom - l_Rect.top, p_bMinimized);
	}
}

void WindowManager::CallWindowManipulated()
{
	LOG("Start window manipulation", "window", Debug)
	if (n_WindowManipulatedCallback)
	{
		n_WindowManipulatedCallback();
	}
}
}