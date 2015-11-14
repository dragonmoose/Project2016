#include "pch.h"
#include "Hawk/Core.h"
#include "Hawk/Logger.h"
#include "Hawk/Exception.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Hawk::Core& l_Core = Hawk::Core::Instance();
	l_Core.Run();
}
