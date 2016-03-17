#include "pch.h"
#include "SceneManager.h"
#include "Util/Algorithm.h"

namespace Hawk {

SceneManager::SceneManager()
{
	CreateRoot();
}

std::string SceneManager::GetName() const
{
	return "SceneManager";
}

Entity* SceneManager::GetRoot()
{
	return m_Root.get();
}

void SceneManager::Initialize()
{
	for (auto& l_Entity : m_Entities)
	{
		l_Entity->Initialize();
	}
}

void SceneManager::RegisterEvents(EventManager& p_EventManager)
{
	for (auto& l_Entity : m_Entities)
	{
		l_Entity->RegisterEvents(p_EventManager);
	}
}

void SceneManager::Update(const Duration& p_Duration)
{
	PROFILE();
	for (auto& l_Entity : m_Entities)
	{
		l_Entity->Update(p_Duration);
	}
}

void SceneManager::AddToScene(Entity::EntityPtr p_Entity)
{
	ASSERT(!hwk::contains(m_Entities, p_Entity), "Scene already contains entity. EntityName=" << p_Entity->GetName());

	m_Entities.push_back(p_Entity);
	m_EntityIDMap[p_Entity->GetID()] = p_Entity;
	LOGM("Entity added to scene: " << p_Entity->GetName(), Debug);
}

void SceneManager::RemoveFromScene(Entity::EntityPtr p_Entity)
{
	auto l_Itr = std::find(m_Entities.begin(), m_Entities.end(), p_Entity);
	ASSERT(l_Itr != m_Entities.end(), "Entity not part of scene. EntityName=" << p_Entity->GetName());

	m_Entities.erase(l_Itr);
	m_EntityIDMap.erase(p_Entity->GetID());
	LOGM("Entity removed from scene: " << p_Entity->GetName(), Debug);
}

void SceneManager::Clear()
{
	for (auto& l_Entity : m_Entities)
	{
		l_Entity->SetParent(nullptr);
	}
	m_Entities.clear();
	m_EntityIDMap.clear();
	Entity::ResetIDCounter();
	LOGM("Scene cleared", Info);

	CreateRoot();
}

void SceneManager::CreateRoot()
{
	m_Root.reset();
	ASSERT(!m_Root, "Root node should have zero ref count");
	m_Root = std::make_shared<Entity>("RootNode");
	m_Root->AddToScene(this);
}

}