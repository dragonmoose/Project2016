#pragma once
#include "Entity.h"
#include "Base/Module.h"

namespace Hawk {

class SceneManager : public Module
{
public:
	SceneManager();
	std::string GetName() const override;
	Entity& GetRoot();

#ifdef HAWK_DEBUG
	void InitializeConsole() override;
#endif

	void Initialize() override;
	void RegisterEvents(EventManager& p_EventManager) override;
	void Update(const Duration& p_Duration) override;
	void AddToScene(Entity::EntityPtr_t p_Entity);
	void RemoveFromScene(Entity::EntityPtr_t p_Entity);

	void CmdPrint();

private:
	Entity m_Root;
	Entity::EntityVec_t m_Entities;
	
};

}