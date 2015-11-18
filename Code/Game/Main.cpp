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
		l_Core.RegisterThread("Logic");
		l_Core.RegisterThread("Logic2");
		l_Core.AddSystem(std::make_unique<TestSystem>(), "Logic");
		l_Core.AddSystem(std::make_unique<TestSystem2>(), "Logic2");
		l_Core.Run();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
}