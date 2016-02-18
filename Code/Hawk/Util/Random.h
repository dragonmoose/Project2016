#pragma once

namespace Hawk {
namespace Random
{
	void	Initialize(unsigned int p_uiSeed = 0);
	float	GetFloat(float p_fMin, float p_fMax);
	int		GetInt(int p_iMin, int p_iMax);
	bool	GetBool();
	float	GetNormalized();
}
}