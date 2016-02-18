#pragma once

namespace Hawk {
namespace Random
{
	void Initialize(unsigned int p_uiSeed = 0);
	float Get(float p_uiMin, float p_uiMax);
	float GetNormalized();
}
}