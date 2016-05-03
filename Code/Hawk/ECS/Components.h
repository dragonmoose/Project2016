#pragma once
#include "Util.h"
#include "glm/glm.hpp"

namespace Hawk {
namespace ECS {

struct CtPosition
{
	DECL_CMP(0);
	CtPosition() : m_Vec(0) {}
	CtPosition(const glm::vec3& p_Vec) : m_Vec(p_Vec) {}
	glm::vec3 m_Vec;
};

struct CtCamera
{
	DECL_CMP(1);
	CtCamera() : m_fFOV(0.0f) {}
	CtCamera(float p_fFOV) : m_fFOV(p_fFOV) {}
	float m_fFOV;
};

struct CtKillTime
{
	DECL_CMP(2);
	CtKillTime() {};
	CtKillTime(Time p_Time) : m_Time(p_Time) {}
	Time m_Time;
};

struct CtKeyInput
{
	DECL_CMP(3);
};

struct CtSceneChild
{
	DECL_CMP(4);
	CtSceneChild() : m_Parent(EntityID_Invalid) {}
	CtSceneChild(EntityID p_ID) : m_Parent(p_ID) {}
	EntityID m_Parent;
};

struct CtSceneParent
{
	DECL_CMP(5);
	std::vector<EntityID> m_Children;
};

}
}