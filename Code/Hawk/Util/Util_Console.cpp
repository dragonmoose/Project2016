#include "pch.h"
#include "Util_Console.h"
#include "Random.h"
#include "Console/ConsoleCommandManager.h"

namespace Hawk {
namespace Util_Console
{
	void SetRandSeed(unsigned int p_uiSeed);
	void GetRand(float p_uiMin, float p_uiMax, unsigned int p_uiCount);
}

void Util_Console::Register(ConsoleCommandManager* p_pManager, std::shared_ptr<Dispatcher> p_Dispatcher)
{
	p_pManager->Register("test.setRandSeed", &Util_Console::SetRandSeed, p_Dispatcher.get(), "Sets the random number generator seed", "[seed]");
	p_pManager->Register("test.getRand", &Util_Console::GetRand, p_Dispatcher.get(), "Gets random numbers", "[min] [max] [count]");
}

void Util_Console::SetRandSeed(unsigned int p_uiSeed)
{
	Random::Initialize(p_uiSeed);
}

void Util_Console::GetRand(float p_uiMin, float p_uiMax, unsigned int p_uiCount)
{
	CONSOLE_WRITE_SCOPE();
	static const unsigned int sc_uiMaxCols = 10;
	unsigned int l_uiCurrCol = 0;
	std::cout << "\n";
	for (unsigned int i = 0; i < p_uiCount; i++)
	{
		std::cout << Random::GetFloat(p_uiMin, p_uiMax) << " ";

		if (l_uiCurrCol == sc_uiMaxCols)
		{
			l_uiCurrCol = 0;
			std::cout << "\n";
		}
		else
		{
			l_uiCurrCol++;
		}
	}
	std::cout << "\n\n";
}

}