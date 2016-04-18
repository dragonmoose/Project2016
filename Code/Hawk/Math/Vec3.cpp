#include "pch.h"
#include "Vec3.h"

namespace Hawk {

Vec3::Vec3(float p_fX, float p_fY, float p_fZ)
: m_Vec(p_fX, p_fY, p_fZ)
{
}

Vec3::Vec3(float p_fValue)
: m_Vec(p_fValue)
{
}

}