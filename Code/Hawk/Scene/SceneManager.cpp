#include "pch.h"
#include "SceneManager.h"
#include "Debug/Assert.h"
#include "Util/Algorithm.h"

namespace Hawk {

SceneManager::SceneManager()
: m_Root("RootNode")
{
	Entity::EntityPtr_t l_Test1 = std::make_shared<Entity>();
	Entity::EntityPtr_t l_Test2 = std::make_shared<Entity>("Named1");
	Entity::EntityPtr_t l_Test3 = std::make_shared<Entity>("Named2");
	Entity::EntityPtr_t l_Test4 = std::make_shared<Entity>("Named3");
	Entity::EntityPtr_t l_Test5 = std::make_shared<Entity>();

	m_Root.AddChild(l_Test1);
	m_Root.AddChild(l_Test2);
	l_Test2->AddChild(l_Test3);
	l_Test2->AddChild(l_Test4);
	m_Root.AddChild(l_Test5);

}

std::string SceneManager::GetName() const
{
	return "SceneManager";
}

Entity& SceneManager::GetRoot()
{
	return m_Root;
}

#ifdef HAWK_DEBUG
void SceneManager::InitializeConsole()
{
	RegisterConsole("scene.print", this, &SceneManager::CmdPrint, "Output scene manager tree to console", "");
}
#endif

void SceneManager::Initialize()
{
	m_Root.Initialize();
	for (auto& l_Entity : m_Entities)
	{
		l_Entity->Initialize();
	}
}

void SceneManager::RegisterEvents(EventManager& p_EventManager)
{
	m_Root.RegisterEvents(p_EventManager);
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
}

void SceneManager::RemoveFromScene(Entity::EntityPtr_t p_Entity)
{
	auto l_Itr = std::find(m_Entities.begin(), m_Entities.end(), p_Entity);
	ASSERT(l_Itr != m_Entities.end(), "Entity not part of scene. EntityName=" << p_Entity->GetName());
	m_Entities.erase(l_Itr);
}

void SceneManager::CmdPrint()
{
	CONSOLE_WRITE_SCOPE();
	std::cout << "\n";
	m_Root.DebugPrint(0);
}

}