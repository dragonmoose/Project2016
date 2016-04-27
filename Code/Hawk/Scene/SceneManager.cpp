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

SceneNode* SceneManager::GetRoot()
{
	return m_Root.get();
}

void SceneManager::Initialize()
{
	for (auto& l_SceneNode : m_Entities)
	{
		l_SceneNode->Initialize();
	}
}

void SceneManager::RegisterEvents(EventManager& p_EventManager)
{
	for (auto& l_SceneNode : m_Entities)
	{
		l_SceneNode->RegisterEvents(p_EventManager);
	}
}

void SceneManager::Update(const Duration& p_Duration)
{
	PROFILE();
	for (auto& l_SceneNode : m_Entities)
	{
		l_SceneNode->Update(p_Duration);
	}
}

void SceneManager::AddToScene(SceneNode::SceneNodePtr p_SceneNode)
{
	ASSERT(!hwk::contains(m_Entities, p_SceneNode), "Scene already contains SceneNode. SceneNodeName=" << p_SceneNode->GetName());

	m_Entities.push_back(p_SceneNode);
	m_SceneNodeIDMap[p_SceneNode->GetID()] = p_SceneNode;
	LOGM("SceneNode added to scene: " << p_SceneNode->GetName(), Debug);
}

void SceneManager::RemoveFromScene(SceneNode::SceneNodePtr p_SceneNode)
{
	auto l_Itr = std::find(m_Entities.begin(), m_Entities.end(), p_SceneNode);
	ASSERT(l_Itr != m_Entities.end(), "SceneNode not part of scene. SceneNodeName=" << p_SceneNode->GetName());

	m_Entities.erase(l_Itr);
	m_SceneNodeIDMap.erase(p_SceneNode->GetID());
	LOGM("SceneNode removed from scene: " << p_SceneNode->GetName(), Debug);
}

void SceneManager::Clear()
{
	for (auto& l_SceneNode : m_Entities)
	{
		l_SceneNode->SetParent(nullptr);
	}
	m_Entities.clear();
	m_SceneNodeIDMap.clear();
	SceneNode::ResetIDCounter();
	LOGM("Scene cleared", Info);

	CreateRoot();
}

void SceneManager::CreateRoot()
{
	m_Root.reset();
	ASSERT(!m_Root, "Root node should have zero ref count");
	m_Root = std::make_shared<SceneNode>("RootNode");
	m_Root->AddToScene(this);
}

}