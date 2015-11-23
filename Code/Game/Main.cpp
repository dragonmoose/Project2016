#include "pch.h"
#include "Hawk/Core.h"
#include "TestSystem.h"
#include "TestSystem2.h"
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Hawk::Core l_Core;
		for (int i = 0; i < 100; i++)
		{
			l_Core.RegisterThread("Logic" + std::to_string(i));
		}
		l_Core.RegisterThread("Main");
		
		for (int i = 0; i < 100; i++)
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