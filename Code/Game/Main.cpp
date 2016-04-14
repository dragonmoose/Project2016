#include "pch.h"
#include "Hawk/Base/Core.h"
#include "Hawk/System/Version.h"
#include "TestModule.h"
#include "TestModule2.h"
#include <map>
#include <boost/lexical_cast.hpp>

int WINAPI WinMain(HINSTANCE p_hInstance, HINSTANCE /*p_hPrevInstance*/, LPSTR /*p_lpCmdLine*/, int /*p_nCmdShow*/)
{
	Hawk::CoreSettings l_Settings;
	l_Settings.m_bConsole = true;
	l_Settings.m_hInstance = p_hInstance;
	l_Settings.m_AppName = "TestApp";
	l_Settings.m_AppVersion = Hawk::Version(1, 0, 0);
	Hawk::Core l_Core(l_Settings);
	try
	{
		l_Core.Initialize();

		Hawk::ThreadID l_Thread1 = l_Core.CreateModuleThread("Thread1");
		Hawk::ThreadID l_Thread2 = l_Core.CreateModuleThread("Thread2");
		l_Core.AddModule<TestModule>(l_Thread1);
		l_Core.AddModule<TestModule2>(l_Thread2);
		//l_Core.RemoveModule(432);
		l_Core.Run();
	}
	catch (Hawk::Exception& e)
	{
		FATAL_EXCEPTION(e, "game");
	}
}