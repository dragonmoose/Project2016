#pragma once

namespace Hawk {
namespace Random
{
	void	Initialize(uint32 p_uiSeed = 0);
	float	GetFloat(float p_fMin, float p_fMax);
	int32	GetInt(int32 p_iMin, int32 p_iMax);
	bool	GetBool();
	float	GetNormalized();
}
}