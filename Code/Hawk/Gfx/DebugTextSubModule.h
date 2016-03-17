#pragma once
#include "Base/SubModule.h"
#include <memory>

namespace Hawk {
namespace Gfx {

class IRenderingAPI;

class DebugTextSubModule : public SubModule
{
public:
	DebugTextSubModule(std::shared_ptr<IRenderingAPI>& p_API);

	std::string GetName() const override;

	void RegisterEvents(EventManager& p_EventManager) override;
#ifdef HAWK_DEBUG
	void InitializeConsole(ScopedConsoleCommands* p_Console) override;
#endif
	void Update(const Duration& p_Duration) override;

	void UpdateValue(const std::string& p_Label, const std::string& p_Value, const std::string& p_Category);
	void RemoveValue(const std::string& p_Label, const std::string& p_Category);

private:
	void CmdSetActiveCategory(const std::string& p_Category);
	void CmdList();
	std::shared_ptr<IRenderingAPI> m_API;
	using Category = std::string;

	struct Node
	{
		Node(const std::string& p_Label, const std::string& p_Value)
		: m_Label(p_Label)
		, m_Value(p_Value) {}

		std::string m_Label;
		std::string m_Value;
	};
	using NodeMap = std::unordered_map<Category, std::vector<Node>>;
	NodeMap m_Map;
};

}
}