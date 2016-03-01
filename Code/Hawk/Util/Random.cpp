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

void Random::Initialize(UINT32 p_uiSeed)
{
	UINT32 l_uiSeed = p_uiSeed;
	if (l_uiSeed == 0)
	{
		l_uiSeed = static_cast<UINT32>(std::chrono::system_clock::now().time_since_epoch().count());
	}
	n_Generator.seed(l_uiSeed);
	LOG_IF(p_uiSeed == 0, "Random number generator initialized from system clock. Seed=" << l_uiSeed, "core", Info);
	LOG_IF(p_uiSeed != 0, "Random number generator initialized from custom value. Seed=" << l_uiSeed, "core", Info);
}

float Random::GetFloat(float p_fMin, float p_fMax)
{
	ASSERT(p_fMin < p_fMax, "Argument error. Min=" << p_fMin << " Max=" << p_fMax);
	return Math::Lerp(p_fMin, p_fMax, GetNormalized());
}

int32_t Random::GetInt(int32_t p_iMin, int32_t p_iMax)
{
	ASSERT(p_iMin < p_iMax, "Argument error. Min=" << p_iMin << " Max=" << p_iMax);
	return Math::Lerp(p_iMin, p_iMax, GetNormalized());
}

bool Random::GetBool()
{
	return GetNormalized() >= 0.5f ? true : false;
}

float Random::GetNormalized()
{
	return n_Generator() / c_fMaxValue;
}

}