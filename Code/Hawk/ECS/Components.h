#pragma once
#include "glm/glm.hpp"

namespace Hawk {
namespace ECS {

struct PositionComponent
{
	glm::vec3 m_Vec;
};

struct CameraComponent
{
	float m_fFOV;
};

struct KillTimeComponent
{
	Time m_Time;
};

}
}