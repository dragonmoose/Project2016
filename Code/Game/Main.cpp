#include "pch.h"
#include "Hawk/Core.h"
#include "Hawk/Logger.h"
#include "Hawk/Exception.h"
#include <cassert>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	assert(Hawk::Core::Initialize());
	Hawk::Core::Run();
}