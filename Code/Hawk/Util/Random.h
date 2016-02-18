#pragma once

namespace Hawk {
namespace Random
{
	void	Initialize(UINT32 p_uiSeed = 0);
	float	GetFloat(float p_fMin, float p_fMax);
	INT32	GetInt(INT32 p_iMin, INT32 p_iMax);
	bool	GetBool();
	float	GetNormalized();
}
}