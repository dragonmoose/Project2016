#pragma once
#include "glm/glm.hpp"

namespace Hawk {
namespace ECS {

struct PositionComponent
{
	glm::vec3 m_Vec;
	static const ComponentID ID = 0;
};

struct CameraComponent
{
	float m_fFOV;
	static const ComponentID ID = 1;
};

struct KillTimeComponent
{
	Time m_Time;
	static const ComponentID ID = 2;
};

struct KeyInputComponent
{
	static const ComponentID ID = 3;
};

}
}