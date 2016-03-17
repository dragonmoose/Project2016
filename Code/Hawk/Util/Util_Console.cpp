#include "pch.h"
#include "Util_Console.h"
#include "Random.h"
#include "Console/ConsoleCommandManager.h"

namespace Hawk {
namespace Util_Console
{
	void SetRandSeed(uint32 p_uiSeed);
	void GetRand(float p_fMin, float p_fMax, uint32 p_uiCount);
}

#ifdef HAWK_DEBUG
void Util_Console::Register(ConsoleCommandManager* p_pManager, std::shared_ptr<Dispatcher> p_Dispatcher)
{
	p_pManager->Register("test.setRandSeed", &Util_Console::SetRandSeed, p_Dispatcher.get(), "Sets the random number generator seed", "[seed]");
	p_pManager->Register("test.getRand", &Util_Console::GetRand, p_Dispatcher.get(), "Gets random numbers", "[min] [max] [count]");
}
#endif

void Util_Console::SetRandSeed(uint32 p_uiSeed)
{
	Random::Initialize(p_uiSeed);
}

void Util_Console::GetRand(float p_fMin, float p_fMax, uint32 p_uiCount)
{
	CONSOLE_WRITE_SCOPE();
	static const uint32 sc_uiMaxCols = 10;
	uint32 l_uiCurrCol = 0;
	std::cout << "\n";
	for (uint32 i = 0; i < p_uiCount; i++)
	{
		std::cout << Random::GetInt(static_cast<int32>(p_fMin), static_cast<int32>(p_fMax)) << " ";

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