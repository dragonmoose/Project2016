#include "pch.h"
#include "SceneManager.h"
#include "Util/Random.h"

#ifdef HAWK_DEBUG
namespace Hawk {

void SceneManager::InitializeConsole(ScopedConsoleCommands* p_Console)
{
	p_Console->Register("scene.print", this, &SceneManager::CmdPrint, "Output scene manager tree to console", "");
	p_Console->Register("sceneNode.remove", this, &SceneManager::CmdRemoveSceneNode, "Removes SceneNode", "[SceneNodeID] [parentID]");
	p_Console->Register("sceneNode.reparent", this, &SceneManager::CmdReparentSceneNode, "Reparents SceneNode", "[SceneNodeID] [newParentID]");
	p_Console->Register("sceneNode.ascenstorOf", this, &SceneManager::CmdAncestorOf, "Checks if first SceneNode is ancestor of second SceneNode", "[SceneNodeID] [SceneNodeID]");
	p_Console->Register("scene.clear", this, &SceneManager::CmdClear, "Clears the scene", "");
	p_Console->Register("scene.populate", this, &SceneManager::CmdPopulate, "Adds test SceneNode hierarchy", "[maxDepth] [max entities per depth level]");
}

void SceneManager::CmdPrint()
{
	CONSOLE_WRITE_SCOPE();
	std::cout << "\n";
	m_Root->DebugPrint(0);
	std::cout << "\n";
}

void SceneManager::CmdRemoveSceneNode(SceneNodeID p_SceneNodeID)
{
	try
	{
		auto l_Itr = m_SceneNodeIDMap.find(p_SceneNodeID);
		THROW_IF(l_Itr == m_SceneNodeIDMap.end(), "No SceneNode in scene with ID=" << p_SceneNodeID);

		const SceneNode::SceneNodePtr l_SceneNode = l_Itr->second;
		l_SceneNode->GetParent()->RemoveChild(l_SceneNode);
		std::cout << "SceneNode removed.\n";
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION_CONSOLE(e);
	}
}

void SceneManager::CmdReparentSceneNode(SceneNodeID p_SceneNodeID, SceneNodeID p_NewParentID)
{
	try
	{
		THROW_IF(p_SceneNodeID == p_NewParentID, "Cannot add SceneNode to itself");

		auto l_Itr = m_SceneNodeIDMap.find(p_SceneNodeID);
		THROW_IF(l_Itr == m_SceneNodeIDMap.end(), "No SceneNode in scene with ID=" << p_SceneNodeID);
		const SceneNode::SceneNodePtr l_SceneNode = l_Itr->second;

		auto l_ParentItr = m_SceneNodeIDMap.find(p_NewParentID);
		THROW_IF(l_ParentItr == m_SceneNodeIDMap.end(), "No SceneNode in scene with ID=" << p_NewParentID);
		const SceneNode::SceneNodePtr l_ParentSceneNode = l_ParentItr->second;

		THROW_IF(l_SceneNode->IsChildOf(l_ParentSceneNode->GetID()), "Already child of specified SceneNode");

		l_ParentSceneNode->AddChild(l_SceneNode);
		std::cout << "Child reparented.\n";
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION_CONSOLE(e);
	}
}

void SceneManager::CmdAncestorOf(SceneNodeID p_SceneNodeID1, SceneNodeID p_SceneNodeID2)
{
	try
	{
		auto l_Itr1 = m_SceneNodeIDMap.find(p_SceneNodeID1);
		THROW_IF(l_Itr1 == m_SceneNodeIDMap.end(), "No SceneNode in scene with ID=" << p_SceneNodeID1);
		const SceneNode::SceneNodePtr l_SceneNode1 = l_Itr1->second;

		auto l_Itr2 = m_SceneNodeIDMap.find(p_SceneNodeID2);
		THROW_IF(l_Itr2 == m_SceneNodeIDMap.end(), "No SceneNode in scene with ID=" << p_SceneNodeID2);
		const SceneNode::SceneNodePtr l_SceneNode2 = l_Itr2->second;

		std::cout << "Result: " << l_SceneNode1->IsAncestorOf(l_SceneNode2) << "\n\n";
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION_CONSOLE(e);
	}
}

namespace
{
	SceneNode::SceneNodePtr AddSceneNode(SceneNode::SceneNodePtr p_Parent)
	{
		SceneNode::SceneNodePtr l_SceneNode = std::make_shared<SceneNode>();
		p_Parent->AddChild(l_SceneNode);
		return l_SceneNode;
	}

	void AddEntities(SceneNode::SceneNodePtr p_Parent, int32 p_iDepth, int32 p_iLevelMax)
	{
		if (p_iDepth >= 0)
		{
			int32 l_iNumNodes = Random::GetInt(1, p_iLevelMax);
			for (int32 i = 0; i < l_iNumNodes; i++)
			{
				AddEntities(AddSceneNode(p_Parent), p_iDepth - 1, p_iLevelMax);
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

void SceneManager::CmdPopulate(int32 p_iDepth, int32 p_iLevelMax)
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
#endif