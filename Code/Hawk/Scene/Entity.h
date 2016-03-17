#pragma once
#include "Component.h"
#include "Threading/Mutex.h"
#include "System/Types.h"
#pragma warning (push)
#pragma warning (disable:4201)	// disable nonstandard extension for glm (nameless struct/union)
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#pragma warning (pop)
#include <memory>
#include <string>

namespace Hawk {

class EventManager;
class SceneManager;

class Entity : public std::enable_shared_from_this<Entity>
{
public:
	using EntityPtr = std::shared_ptr<Entity>;
	using EntityVec = std::vector<EntityPtr>;

	Entity();
	Entity(const std::string& p_Name);
	~Entity();
	void SetParent(EntityPtr p_Parent);

	template<class T, class... Args>
	T& AddComponent(Args&&... p_Args)
	{
		m_Components.emplace_back(p_Args);
		return &m_Components.back();
	}

	const std::string& GetName() const;

	void AddChild(EntityPtr p_Entity);
	void RemoveChild(EntityPtr p_Entity);
	EntityPtr GetParent() const;
	bool HasParent() const;
	bool HasChild(EntityID p_ID) const;
	bool IsChildOf(EntityID p_ID) const;
	bool IsAncestorOf(EntityPtr p_Entity) const;
	void AddToScene(SceneManager* p_pSceneManager);

	void Initialize();
	void RegisterEvents(EventManager& p_EventManager);
	void Update(const Duration& p_Duration);
	EntityID GetID() const;

	const glm::mat4x4& GetFrameWorldMatrix();

	void DebugPrint(uint32 p_uiDepth) const;
	static void ResetIDCounter();

private:
	void DetachChild(EntityPtr p_Entity);
	bool InScene() const;
	void RemoveFromScene();

	std::string m_Name;
	bool m_bInitialized;

	static EntityID s_NextID;

	using Components = std::vector<Component>;
	Components m_Components;

	EntityPtr m_Parent;

	using Children = std::vector<EntityPtr>;
	Children m_Children;

	SceneManager* m_pSceneManager;

	glm::vec3 m_Position;
	glm::quat m_Rotation;
	glm::mat4x4 m_FrameWorldMatrix;
	EntityID m_ID;
};

}