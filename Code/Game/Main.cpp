#include "pch.h"
#include "Hawk/Core.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Hawk::Core::Get();
	LOG_WARNING("warn test blabla");
	LOG_ERROR("errrerererere r ere n test blabla");
	LOG_INFO("errrerererere r ere n test blabla");
	while (1);
}
