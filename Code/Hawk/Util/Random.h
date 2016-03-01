#pragma once

namespace Hawk {
namespace Random
{
	void	Initialize(UINT32 p_uiSeed = 0);
	float	GetFloat(float p_fMin, float p_fMax);
	int32_t	GetInt(int32_t p_iMin, int32_t p_iMax);
	bool	GetBool();
	float	GetNormalized();
}
}