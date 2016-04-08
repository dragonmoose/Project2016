#include "pch.h"
#include "Math.h"

namespace Hawk {

float Math::Deg2Rad(float p_fDeg)
{
	static const float sc_fConvFac = HAWK_PI / 180.0f;
	return p_fDeg * sc_fConvFac;
}

}