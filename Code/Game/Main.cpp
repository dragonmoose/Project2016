#include "pch.h"
#include "Hawk/Core.h"
#include "Hawk/EventMonitoringSystem.h"
#include "TestSystem.h"
#include "TestSystem2.h"
#include <iostream>

int WINAPI WinMain(HINSTANCE p_hInstance, HINSTANCE p_hPrevInstance, LPSTR p_lpCmdLine, int p_nCmdShow)
{
	try
	{
		Hawk::Core l_Core;
		l_Core.OpenWindow(p_hInstance, "TestApp");
		for (int i = 0; i < 100; i++)
		{
			l_Core.RegisterThread("Logic" + std::to_string(i));
		}
		l_Core.RegisterThread("EventListener");
		l_Core.AddSystem<Hawk::EventMonitoringSystem>("EventListener");
		l_Core.RegisterThread("Main");
		
		for (int i = 0; i < 3; i++)
		{
			l_Core.AddSystem<TestSystem>("Logic" + std::to_string(i));
		}
		l_Core.AddSystem<TestSystem2>("Main");
		l_Core.Run();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
}