#pragma once
#include "SceneNode.h"
#include "Base/Module.h"
#include "System/Types.h"

namespace Hawk {

class SceneManager : public Module
{
public:
	SceneManager();
	std::string GetName() const override;
	SceneNode* GetRoot();

#ifdef HAWK_DEBUG
	void InitializeConsole(ScopedConsoleCommands* p_Console) override;

	void CmdPrint();
	void CmdRemoveSceneNode(SceneNodeID p_SceneNodeID);
	void CmdReparentSceneNode(SceneNodeID p_SceneNodeID, SceneNodeID p_NewParentID);
	void CmdAncestorOf(SceneNodeID p_SceneNodeID1, SceneNodeID p_SceneNodeID2);
	void CmdClear();
	void CmdPopulate(int32 p_iDepth, int32 p_iLevelMax);
#endif

	void AddToScene(SceneNode::SceneNodePtr p_SceneNode);
	void RemoveFromScene(SceneNode::SceneNodePtr p_SceneNode);
	void Clear();

private:
	void CreateRoot();

	SceneNode::SceneNodePtr m_Root;
	SceneNode::SceneNodeVec m_Entities;

	using SceneNodeIDMap = std::unordered_map<SceneNodeID, SceneNode::SceneNodePtr>;
	SceneNodeIDMap m_SceneNodeIDMap;	
};

}