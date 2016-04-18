#pragma once
#include "IShape.h"
#include "Math/Vec3.h"

namespace Hawk {

class Box : public IShape
{
public:
	Box(const Vec3& p_Min, const Vec3& p_Max);
	bool IsEnclosing(const IShape& p_Other) const override;

private:
	Vec3 m_Min;
	Vec3 m_Max;
};

}