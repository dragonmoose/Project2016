#pragma once
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

class SceneNode : public std::enable_shared_from_this<SceneNode>
{
public:
	using SceneNodePtr = std::shared_ptr<SceneNode>;
	using SceneNodeVec = std::vector<SceneNodePtr>;

	SceneNode();
	SceneNode(const std::string& p_Name);
	~SceneNode();
	void SetParent(SceneNodePtr p_Parent);

	const std::string& GetName() const;

	void AddChild(SceneNodePtr p_SceneNode);
	void RemoveChild(SceneNodePtr p_SceneNode);
	SceneNodePtr GetParent() const;
	bool HasParent() const;
	bool HasChild(SceneNodeID p_ID) const;
	bool IsChildOf(SceneNodeID p_ID) const;
	bool IsAncestorOf(SceneNodePtr p_SceneNode) const;
	void AddToScene(SceneManager* p_pSceneManager);

	SceneNodeID GetID() const;

	const glm::mat4x4& GetFrameWorldMatrix();

	void DebugPrint(uint32 p_uiDepth) const;
	static void ResetIDCounter();

private:
	void DetachChild(SceneNodePtr p_SceneNode);
	bool InScene() const;
	void RemoveFromScene();

	std::string m_Name;

	static SceneNodeID s_NextID;

	SceneNodePtr m_Parent;

	using Children = std::vector<SceneNodePtr>;
	Children m_Children;

	SceneManager* m_pSceneManager;

	glm::vec3 m_Position;
	glm::quat m_Rotation;
	glm::mat4x4 m_FrameWorldMatrix;
	SceneNodeID m_ID;
};

}