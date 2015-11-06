#include "pch.h"
#include "Hawk/Core.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Hawk::Core::Get();
	while (1);
}