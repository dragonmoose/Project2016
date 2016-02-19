#include "pch.h"
#include "SceneManager.h"
#include "Util/Random.h"

namespace Hawk {

#ifdef HAWK_DEBUG
void SceneManager::InitializeConsole()
{
	RegisterConsole("scene.print", this, &SceneManager::CmdPrint, "Output scene manager tree to console", "");
	RegisterConsole("entity.remove", this, &SceneManager::CmdRemoveEntity, "Removes entity", "[entityID] [parentID]");
	RegisterConsole("entity.reparent", this, &SceneManager::CmdReparentEntity, "Reparents entity", "[entityID] [newParentID]");
	RegisterConsole("entity.ascenstorOf", this, &SceneManager::CmdAncestorOf, "Checks if first entity is ancestor of second entity", "[entityID] [entityID]");
	RegisterConsole("scene.clear", this, &SceneManager::CmdClear, "Clears the scene", "");
	RegisterConsole("scene.populate", this, &SceneManager::CmdPopulate, "Adds test entity hierarchy", "[maxDepth] [max entities per depth level]");
}
#endif

void SceneManager::CmdPrint()
{
	CONSOLE_WRITE_SCOPE();
	std::cout << "\n";
	m_Root->DebugPrint(0);
	std::cout << "\n";
}

void SceneManager::CmdRemoveEntity(EntityID_t p_EntityID)
{
	try
	{
		auto l_Itr = m_EntityIDMap.find(p_EntityID);
		THROW_IF(l_Itr == m_EntityIDMap.end(), "No entity in scene with ID=" << p_EntityID);

		const Entity::EntityPtr_t l_Entity = l_Itr->second;
		l_Entity->GetParent()->RemoveChild(l_Entity);
		std::cout << "Entity removed.\n";
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION_CONSOLE(e);
	}
}

void SceneManager::CmdReparentEntity(EntityID_t p_EntityID, EntityID_t p_NewParentID)
{
	try
	{
		THROW_IF(p_EntityID == p_NewParentID, "Cannot add entity to itself");

		auto l_Itr = m_EntityIDMap.find(p_EntityID);
		THROW_IF(l_Itr == m_EntityIDMap.end(), "No entity in scene with ID=" << p_EntityID);
		const Entity::EntityPtr_t l_Entity = l_Itr->second;

		auto l_ParentItr = m_EntityIDMap.find(p_NewParentID);
		THROW_IF(l_ParentItr == m_EntityIDMap.end(), "No entity in scene with ID=" << p_NewParentID);
		const Entity::EntityPtr_t l_ParentEntity = l_ParentItr->second;

		THROW_IF(l_Entity->IsChildOf(l_ParentEntity->GetID()), "Already child of specified entity");

		l_ParentEntity->AddChild(l_Entity);
		std::cout << "Child reparented.\n";
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION_CONSOLE(e);
	}
}

void SceneManager::CmdAncestorOf(EntityID_t p_EntityID1, EntityID_t p_EntityID2)
{
	try
	{
		auto l_Itr1 = m_EntityIDMap.find(p_EntityID1);
		THROW_IF(l_Itr1 == m_EntityIDMap.end(), "No entity in scene with ID=" << p_EntityID1);
		const Entity::EntityPtr_t l_Entity1 = l_Itr1->second;

		auto l_Itr2 = m_EntityIDMap.find(p_EntityID2);
		THROW_IF(l_Itr2 == m_EntityIDMap.end(), "No entity in scene with ID=" << p_EntityID2);
		const Entity::EntityPtr_t l_Entity2 = l_Itr2->second;

		std::cout << "Result: " << l_Entity1->IsAncestorOf(l_Entity2) << "\n\n";
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION_CONSOLE(e);
	}
}

namespace
{
	Entity::EntityPtr_t AddEntity(Entity::EntityPtr_t p_Parent)
	{
		Entity::EntityPtr_t l_Entity = std::make_shared<Entity>();
		p_Parent->AddChild(l_Entity);
		return l_Entity;
	}

	void AddEntities(Entity::EntityPtr_t p_Parent, INT32 p_iDepth, INT32 p_iLevelMax)
	{
		if (p_iDepth >= 0)
		{
			INT32 l_iNumNodes = Random::GetInt(1, p_iLevelMax);
			for (INT32 i = 0; i < l_iNumNodes; i++)
			{
				AddEntities(AddEntity(p_Parent), p_iDepth - 1, p_iLevelMax);
			}
		}
	}
}

void SceneManager::CmdClear()
{
	Clear();
	CONSOLE_WRITE_SCOPE();
	std::cout << "Scene cleared.\n";
}

void SceneManager::CmdPopulate(INT32 p_iDepth, INT32 p_iLevelMax)
{
	try
	{
		THROW_IF_NOT(p_iDepth >= 0, "Depth cannot be negative");
		THROW_IF_NOT(p_iDepth <= 5, "Depth cannot exceed 5");
		THROW_IF_NOT(p_iLevelMax > 0, "LevelMax needs to be at least 1");
		THROW_IF_NOT(p_iLevelMax <= 5, "LevelMax cannot exceed 5");

		Clear();
		AddEntities(m_Root, p_iDepth, p_iLevelMax + 1);
		
		CONSOLE_WRITE_SCOPE();
		std::cout << "-New scene-------------------------------------------------\n";
		m_Root->DebugPrint(0);
		std::cout << "\n\n";
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION_CONSOLE(e);
	}
}
}