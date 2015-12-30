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

		Hawk::ThreadID l_Thread1 = l_Core.CreateModuleThread("Thread1");
		Hawk::ThreadID l_Thread2 = l_Core.CreateModuleThread("Thread2");
		l_Core.AddModule<TestModule>(l_Thread1);
		l_Core.AddModule<TestModule2>(l_Thread2);
		l_Core.Run();
	}
	catch (Hawk::Exception& e)
	{
		std::cout << e.what() << "\n";
	}
}