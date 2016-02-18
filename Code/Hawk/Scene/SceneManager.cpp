#include "pch.h"
#include "SceneManager.h"
#include "Debug/Assert.h"
#include "Util/Algorithm.h"

namespace Hawk {

SceneManager::SceneManager()
: m_Root(std::make_shared<Entity>("RootNode"))
{
	m_Root->AddToScene(this);
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

void SceneManager::AddToScene(Entity::EntityPtr_t p_Entity)
{
	ASSERT(!hwk::contains(m_Entities, p_Entity), "Scene already contains entity. EntityName=" << p_Entity->GetName());

	m_Entities.push_back(p_Entity);
	m_EntityIDMap[p_Entity->GetID()] = p_Entity;
	m_EntityParentMap[p_Entity->GetID()] = p_Entity->GetParent();
}

void SceneManager::RemoveFromScene(Entity::EntityPtr_t p_Entity)
{
	auto l_Itr = std::find(m_Entities.begin(), m_Entities.end(), p_Entity);
	ASSERT(l_Itr != m_Entities.end(), "Entity not part of scene. EntityName=" << p_Entity->GetName());

	m_Entities.erase(l_Itr);
	m_EntityIDMap.erase(p_Entity->GetID());
	m_EntityParentMap.erase(p_Entity->GetID());
}

void SceneManager::Clear()
{
	for (auto& l_Entity : m_Entities)
	{
		l_Entity->SetParent(nullptr);
	}
	m_Entities.clear();
	m_EntityIDMap.clear();
	m_EntityParentMap.clear();
	LOGM("Scene cleared", Info);
}

}