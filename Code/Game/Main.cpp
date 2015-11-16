#include "pch.h"
#include "Hawk/Core.h"
#include "Hawk/Logger.h"
#include "Hawk/Exception.h"
#include "TestSystem.h"
#include "TestSystem2.h"
#include <cassert>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	assert(Hawk::Core::Initialize());
	Hawk::Core::AddSystem(std::make_unique<TestSystem>());
	Hawk::Core::AddSystem(std::make_unique<TestSystem2>());
	Hawk::Core::Run();
}