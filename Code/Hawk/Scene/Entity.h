#pragma once
#include "Component.h"
#include "Threading/Mutex.h"
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <string>

namespace Hawk {

class EventManager;
class SceneManager;

class Entity : public std::enable_shared_from_this<Entity>
{
public:
	using EntityPtr_t = std::shared_ptr<Entity>;
	using EntityVec_t = std::vector<EntityPtr_t>;

	Entity();
	Entity(const std::string& p_Name);

	template<class T, class... Args>
	T& AddComponent(Args&&... p_Args)
	{
		m_Components.emplace_back(p_Args);
		return &m_Components.back();
	}

	const std::string& GetName() const;

	void AddChild(EntityPtr_t p_Entity);
	void RemoveChild(EntityPtr_t p_Entity);
	EntityPtr_t GetParent() const;
	bool HasParent() const;

	void Initialize();
	void RegisterEvents(EventManager& p_EventManager);
	void Update(const Duration& p_Duration);

	const glm::mat4x4& GetFrameWorldMatrix();

	void DebugPrint(unsigned int p_uiDepth) const;

private:
	void DetachChild(EntityPtr_t p_Entity);
	bool InScene() const;
	void AddToScene(SceneManager* p_pSceneManager);
	void RemoveFromScene();

	std::string m_Name;
	bool m_bInitialized;

	static unsigned int s_uiNextObjectID;

	using Components_t = std::vector<Component>;
	Components_t m_Components;

	EntityPtr_t m_Parent;

	using Children_t = std::vector<EntityPtr_t>;
	Children_t m_Children;

	SceneManager* m_pSceneManager;

	glm::vec3 m_Position;
	glm::quat m_Rotation;
	glm::mat4x4 m_FrameWorldMatrix;
	unsigned int m_uiObjectID;
};

}