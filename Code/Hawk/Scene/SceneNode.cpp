#include "pch.h"
#include "SceneNode.h"
#include "SceneManager.h"

namespace Hawk {

SceneNodeID SceneNode::s_NextID = 1;

SceneNode::SceneNode()
: SceneNode(std::string("SceneNode_") + std::to_string(s_NextID))
{
}

SceneNode::SceneNode(const std::string& p_Name)
: m_Name(p_Name)
, m_bInitialized(false)
, m_pSceneManager(nullptr)
, m_ID(s_NextID)
{
	s_NextID++;
}

SceneNode::~SceneNode()
{
	LOG("SceneNode deleted. Name=" << m_Name << " ID=" << m_ID, "scene", Debug);
}

void SceneNode::SetParent(SceneNodePtr p_Parent)
{
	ASSERT(p_Parent.get() != this, "Cannot set parent to itself");
	if (HasParent())
	{
		m_Parent->DetachChild(shared_from_this());
	}
	if (p_Parent)
	{
		p_Parent->AddChild(shared_from_this());
	}
}

const std::string& SceneNode::GetName() const
{
	return m_Name;
}

void SceneNode::AddChild(SceneNodePtr p_SceneNode)
{
	ASSERT(std::find(m_Children.begin(), m_Children.end(), p_SceneNode) == m_Children.end(), "Cannot add SceneNode to itself");
	ASSERT(!p_SceneNode->IsAncestorOf(shared_from_this()), "Cannot add ancestor SceneNode as child");

	if (p_SceneNode->HasParent())
	{
		p_SceneNode->m_Parent->DetachChild(p_SceneNode);
	}

	m_Children.push_back(p_SceneNode);
	p_SceneNode->m_Parent = shared_from_this();

	if (InScene())
	{
		p_SceneNode->AddToScene(m_pSceneManager);
	}
}

void SceneNode::RemoveChild(SceneNodePtr p_SceneNode)
{
	//ASSERT_THREAD("scene");
	DetachChild(p_SceneNode);
	p_SceneNode->RemoveFromScene();
}

SceneNode::SceneNodePtr SceneNode::GetParent() const
{
	return m_Parent;
}

bool SceneNode::HasParent() const
{
	return m_Parent != nullptr;
}

bool SceneNode::HasChild(SceneNodeID p_ID) const
{
	return std::find_if(m_Children.begin(), m_Children.end(), [p_ID](const SceneNodePtr& p_SceneNode) { return p_ID == p_SceneNode->GetID(); }) != m_Children.end();
}

bool SceneNode::IsChildOf(SceneNodeID p_ID) const
{
	return HasParent() ? m_Parent->GetID() == p_ID : false;
}

bool SceneNode::IsAncestorOf(SceneNodePtr p_SceneNode) const
{
	ASSERT(p_SceneNode, "SceneNode cannot be null");
	if (p_SceneNode->IsChildOf(m_ID))
	{
		return true;
	}
	else if (p_SceneNode->HasParent())
	{
		return IsAncestorOf(p_SceneNode->GetParent());
	}
	return false;
}


void SceneNode::AddToScene(SceneManager* p_pSceneManager)
{
	ASSERT(p_pSceneManager, "SceneManager should not be null");
	if (!InScene())
	{
		m_pSceneManager = p_pSceneManager;
		m_pSceneManager->AddToScene(shared_from_this());
	}
}

void SceneNode::Initialize()
{
	ASSERT(!m_bInitialized, "Already initialized");
	for (auto& l_Component : m_Components)
	{
		l_Component.Initialize();
	}
	m_bInitialized = true;
}

void SceneNode::RegisterEvents(EventManager& p_EventManager)
{
	for (auto& l_Component : m_Components)
	{
		l_Component.RegisterEvents(p_EventManager);
	}
}

void SceneNode::Update(const Duration& p_Duration)
{
	for (auto& l_Component : m_Components)
	{
		l_Component.Update(p_Duration);
	}
}

SceneNodeID SceneNode::GetID() const
{
	return m_ID;
}

const glm::mat4x4& SceneNode::GetFrameWorldMatrix()
{
	return m_FrameWorldMatrix;
}

void SceneNode::DebugPrint(uint32 p_uiDepth) const
{
	std::fill_n(std::ostreambuf_iterator<char>(std::cout), p_uiDepth * 3, ' ');
	std::cout << GetName() << "\t#" << m_ID << "\n";
	uint32 l_uiChildDepth = p_uiDepth + 1;
	for (auto& l_Child : m_Children)
	{
		l_Child->DebugPrint(l_uiChildDepth);
	}
}
void SceneNode::ResetIDCounter()
{
	s_NextID = 1;
}

void SceneNode::DetachChild(SceneNodePtr p_SceneNode)
{
	auto l_Itr = std::find(m_Children.begin(), m_Children.end(), p_SceneNode);
	ASSERT(l_Itr != m_Children.end(), "SceneNode is not a child of this SceneNode. Parent=" << m_Name << " Child=" << p_SceneNode->m_Name);

	p_SceneNode->m_Parent = nullptr;
	m_Children.erase(l_Itr);
}

bool SceneNode::InScene() const
{
	return m_pSceneManager != nullptr;
}

void SceneNode::RemoveFromScene()
{
	if (InScene())
	{
		m_pSceneManager->RemoveFromScene(shared_from_this());
	}
}

}