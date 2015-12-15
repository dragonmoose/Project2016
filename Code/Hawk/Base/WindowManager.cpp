#include "pch.h"
#include "Base/WindowManager.h"
#include "Input/InputSystem.h"
#include <memory>

namespace Hawk {

namespace WindowManager
{
	LRESULT CALLBACK WindowProc(HWND p_hWindow, UINT p_Message, WPARAM p_wParam, LPARAM p_lParam);
	const char* n_WindowName = "HawkWindowClass";
	std::unique_ptr<InputSystem> n_InputSystem;
}

void WindowManager::Initialize(std::shared_ptr<EventRouter>& p_EventRouter)
{
	n_InputSystem = std::make_unique<InputSystem>(p_EventRouter);
	LOG("WindowManager initialized", "window", Info);
}

void WindowManager::Open(HINSTANCE p_hInstance, const std::string& p_Name)
{
	WNDCLASSEX l_WC;
	ZeroMemory(&l_WC, sizeof(WNDCLASSEX));

	l_WC.cbSize = sizeof(WNDCLASSEX);
	l_WC.style = CS_HREDRAW | CS_VREDRAW;
	l_WC.lpfnWndProc = WindowProc;
	l_WC.hInstance = p_hInstance;
	l_WC.hCursor = LoadCursor(nullptr, IDC_ARROW);
	l_WC.hbrBackground = (HBRUSH)COLOR_WINDOW;
	l_WC.lpszClassName = n_WindowName;

	THROW_IF_NOT(RegisterClassEx(&l_WC), "Failed to register windows class");

	HWND l_hWindow = CreateWindowEx(
		0,
		n_WindowName,
		p_Name.c_str(),
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		640,
		480,
		nullptr,
		nullptr,
		p_hInstance,
		nullptr
	);
	THROW_IF_NOT(l_hWindow, "Failed to create window");
	ShowWindow(l_hWindow, SW_SHOWNORMAL);
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
		default:
			break;
	}
	return DefWindowProc(p_hWindow, p_Message, p_wParam, p_lParam);
}
}