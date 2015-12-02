#include "pch.h"
#include "Hawk/Base/Core.h"
#include "TestModule.h"
#include "TestModule2.h"
#include <iostream>
#include <map>

namespace CmdMan
{
	std::map<std::string, std::function<void()>> m_Map;
	void Register(const std::string& p_Cmd, std::function<void()> func)
	{
		m_Map[p_Cmd] = func;
	}
	

	void Invoke(const std::string& p_Cmd)
	{
		m_Map[p_Cmd]();
	}
};

void TestFunc()
{
	std::cout << "TestFunc\n";
}

void TestFunc2()
{
	std::cout << "TestFunc2!\n";
}

int WINAPI WinMain(HINSTANCE p_hInstance, HINSTANCE p_hPrevInstance, LPSTR p_lpCmdLine, int p_nCmdShow)
{
	Hawk::Core l_Core;
	CmdMan::Register("cmd.test", &TestFunc);
	CmdMan::Register("cmd.test2", &TestFunc2);

	CmdMan::Invoke("cmd.test2");
	while (1);







	try
	{
		
		l_Core.OpenWindow(p_hInstance, "TestApp");
		for (int i = 0; i < 100; i++)
		{
			l_Core.RegisterThread("Logic" + std::to_string(i));
		}
		l_Core.RegisterThread("EventListener");
		l_Core.RegisterThread("Main");
		
		for (int i = 0; i < 3; i++)
		{
			l_Core.AddModule<TestModule>("Logic" + std::to_string(i));
		}
		l_Core.AddModule<TestModule2>("Main");
		l_Core.Run();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
}