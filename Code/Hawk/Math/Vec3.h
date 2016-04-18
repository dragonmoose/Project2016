#pragma once

namespace Hawk {

class HAWK_DLL_EXPORT Vec3
{
public:
	Vec3(float p_fX, float p_fY, float p_fZ);
	Vec3(float p_fValue);

private:
	glm::vec3 m_Vec;
};

}
