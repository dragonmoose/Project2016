#include "pch.h"
#include "SceneManager.h"

namespace Hawk {

void SceneManager::CmdPrint()
{
	CONSOLE_WRITE_SCOPE();
	std::cout << "\n";
	m_Root->DebugPrint(0);
	std::cout << "\n";
}

void SceneManager::CmdRemoveChild(EntityID_t p_ChildID, EntityID_t p_ParentID)
{
	try
	{
		auto l_ItrChild = m_EntityIDMap.find(p_ChildID);
		THROW_IF(l_ItrChild == m_EntityIDMap.end(), "No entity in scene with ID=" << p_ChildID);

		auto l_ItrParent = m_EntityIDMap.find(p_ParentID);
		THROW_IF(l_ItrParent == m_EntityIDMap.end(), "No entity in scene with ID=" << p_ParentID);

		THROW_IF_NOT(l_ItrParent->second->HasChild(l_ItrChild->second), "Entity " << p_ChildID << " is not child of " << p_ParentID);
		l_ItrParent->second->RemoveChild(l_ItrChild->second);

		std::cout << "Child removed.\n";
	}
	catch (Exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n\n";
	}
}

void SceneManager::CmdReparent(EntityID_t p_ChildID, EntityID_t p_OldParentID, EntityID_t p_NewParentID)
{
	try
	{
		auto l_ItrChild = m_EntityIDMap.find(p_ChildID);
		THROW_IF(l_ItrChild == m_EntityIDMap.end(), "No entity in scene with ID=" << p_ChildID);

		auto l_ItrOldParent = m_EntityIDMap.find(p_OldParentID);
		THROW_IF(l_ItrOldParent == m_EntityIDMap.end(), "No entity in scene with ID=" << p_OldParentID);

		auto l_ItrNewParent = m_EntityIDMap.find(p_NewParentID);
		THROW_IF(l_ItrNewParent == m_EntityIDMap.end(), "No entity in scene with ID=" << p_NewParentID);

		THROW_IF_NOT(l_ItrOldParent->second->HasChild(l_ItrChild->second), "Entity " << p_ChildID << " is not child of " << p_OldParentID);
		THROW_IF(l_ItrChild->second == l_ItrNewParent->second, "Cannot add entity to itself");
		THROW_IF(l_ItrNewParent->second->HasChild(l_ItrChild->second), "Already child of specified entity");

		l_ItrNewParent->second->AddChild(l_ItrChild->second);

		std::cout << "Child reparented.\n";
	}
	catch (Exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n\n";
	}
}

void SceneManager::CmdClear()
{
	for (auto& l_Entity : m_Entities)
	{
		l_Entity->SetParent(nullptr);
	}
	m_Entities.clear();
	m_EntityIDMap.clear();

	std::cout << "Scene cleared.\n";
}

void SceneManager::CmdAddTestEntities()
{
	
	Entity::EntityPtr_t l_Test1 = std::make_shared<Entity>();
	Entity::EntityPtr_t l_Test2 = std::make_shared<Entity>("Named1");
	Entity::EntityPtr_t l_Test3 = std::make_shared<Entity>("Named2");
	Entity::EntityPtr_t l_Test4 = std::make_shared<Entity>("Named3");
	Entity::EntityPtr_t l_Test5 = std::make_shared<Entity>();

	m_Root->AddChild(l_Test1);
	m_Root->AddChild(l_Test2);
	l_Test2->AddChild(l_Test3);
	l_Test2->AddChild(l_Test4);
	m_Root->AddChild(l_Test5);

	std::cout << "Scene populated.\n";
}
}