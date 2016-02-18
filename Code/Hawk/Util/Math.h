#pragma once

namespace Hawk {
namespace Math
{
	template<class T>
	T Lerp(const T& p_Min, const T& p_Max, float p_fInterpolant)
	{
		return static_cast<T>((1.0f - p_fInterpolant) * p_Min + p_fInterpolant * p_Max);
	}
}
}