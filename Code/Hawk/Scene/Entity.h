#pragma once
#include "Component.h"
#include "Threading/Mutex.h"
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <string>

namespace Hawk {

class EventManager;

class Entity
{
public:
	using EntityPtr_t = std::shared_ptr<Entity>;

	Entity();
	template<class T, class... Args>
	T& AddComponent(Args&&... p_Args)
	{
		m_Components.emplace_back(p_Args);
		return &m_Components.back();
	}

	void AddChild(EntityPtr_t p_Entity);
	Entity* GetParent() const;
	bool HasParent() const;

	void Initialize();
	void Update(const Duration& p_Duration);

	const glm::mat4x4& GetFrameWorldMatrix();

private:
	std::string m_Name;
	bool m_bInitialized;

	static unsigned int s_uiNextObjectID;

	using Components_t = std::vector<Component>;
	Components_t m_Components;

	EntityPtr_t m_Parent;

	using Children_t = std::vector<EntityPtr_t>;
	Children_t m_Children;

	glm::vec3 m_Position;
	glm::quat m_Rotation;
	glm::mat4x4 m_FrameWorldMatrix;
};

}