#include "pch.h"
#include "Hawk/Base/Core.h"
#include "TestModule.h"
#include "TestModule2.h"
#include <map>
#include <boost/lexical_cast.hpp>

int WINAPI WinMain(HINSTANCE p_hInstance, HINSTANCE p_hPrevInstance, LPSTR p_lpCmdLine, int p_nCmdShow)
{
	try
	{
		Hawk::Core l_Core;
		l_Core.OpenWindow(p_hInstance, "TestApp");

		l_Core.RegisterThread("Thread_TestModule");
		l_Core.RegisterThread("Thread_TestModule2");
		l_Core.AddModule<TestModule>("Thread_TestModule");
		l_Core.AddModule<TestModule2>("Thread_TestModule2");

		l_Core.Run();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
}