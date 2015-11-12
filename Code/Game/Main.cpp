#include "pch.h"
#include "Hawk/Core.h"
#include "Hawk/Logger.h"
#include "Hawk/Exception.h"
#include <thread>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Hawk::Core& l_Core = Hawk::Core::Instance();
	LOG_INFO("Game startup");

	while (1)
	{
		using namespace std::literals;
		std::this_thread::sleep_for(2s);
		LOG_DEBUG("Test debug log");
	}
}
