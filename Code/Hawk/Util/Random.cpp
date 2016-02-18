#include "pch.h"
#include "Random.h"
#include "Math.h"
#include "Debug/Assert.h"
#include <random>
#include <chrono>

namespace Hawk {
namespace Random
{
	std::mt19937 n_Generator;
	const float c_fMaxValue = static_cast<float>(n_Generator.max());
}

void Random::Initialize(unsigned int p_uiSeed)
{
	unsigned int l_uiSeed = p_uiSeed;
	if (l_uiSeed == 0)
	{
		l_uiSeed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	}
	n_Generator.seed(l_uiSeed);
	LOG_IF(p_uiSeed == 0, "Random number generator initialized from system clock. Seed=" << l_uiSeed, "core", Info);
	LOG_IF(p_uiSeed != 0, "Random number generator initialized from custom value. Seed=" << l_uiSeed, "core", Info);
}

float Random::Get(float p_fMin, float p_fMax)
{
	ASSERT(p_fMin < p_fMax, "Parameter error. Min=" << p_fMin << " Max=" << p_fMax);
	return Math::Lerp(p_fMin, p_fMax, GetNormalized());
}

float Random::GetNormalized()
{
	return n_Generator() / c_fMaxValue;
}

}