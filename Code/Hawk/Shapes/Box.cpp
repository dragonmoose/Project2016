#include "pch.h"
#include "Box.h"

namespace Hawk {

Box::Box(const Vec3& p_Min, const Vec3& p_Max)
: m_Min(p_Min)
, m_Max(p_Max)
{

}

bool Box::IsEnclosing(const IShape& /*p_Other*/) const
{
	NOT_IMPLEMENTED();
	return false;
}



}