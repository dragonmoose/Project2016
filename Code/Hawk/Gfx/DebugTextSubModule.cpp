#include "pch.h"
#include "DebugTextSubModule.h"
#include "GfxDebugEvents.h"
#include "IRenderingAPI.h"

namespace Hawk {
namespace Gfx {

DebugTextSubModule::DebugTextSubModule(std::shared_ptr<IRenderingAPI>& p_API)
: m_API(p_API)
{
}

std::string DebugTextSubModule::GetName() const
{
	return "debug_text";
}

void DebugTextSubModule::RegisterEvents(EventManager& p_EventManager)
{
	p_EventManager.Register<UpdateDebugTextEvent>([this](const UpdateDebugTextEvent& p_Event)
	{
		UpdateValue(p_Event.m_Label, p_Event.m_Value, p_Event.m_Category);
	});

	p_EventManager.Register<RemoveDebugTextEvent>([this](const RemoveDebugTextEvent& p_Event)
	{
		RemoveValue(p_Event.m_Label, p_Event.m_Category);
	});
}

void DebugTextSubModule::InitializeConsole()
{
	RegisterConsole("gfx.dbgStatSet", this, &DebugTextSubModule::CmdSetActiveCategory, "Sets the currenty displayed debug category", "[category]", false);
	RegisterConsole("gfx.dbgStatList", this, &DebugTextSubModule::CmdList, "Lists categories and variables", "");
}

void DebugTextSubModule::Update(const Duration& p_Duration)
{
	std::string l_Text;
	if (!m_ActiveCategory.empty())
	{
		auto l_Itr = m_Map.find(m_ActiveCategory);
		if (l_Itr != m_Map.cend())
		{
			for (const auto& l_Node : l_Itr->second)
			{
				l_Text += l_Node.m_Label + ": " + l_Node.m_Value + "\n";
			}
		}
		else
		{
			std::cout << "Failed to find debug text category - clearing category.\n\n";
			m_ActiveCategory.clear();
		}
	}
	m_API->SetDebugText(l_Text);
}

void DebugTextSubModule::UpdateValue(const std::string& p_Label, const std::string& p_Value, const std::string& p_Category)
{
	std::vector<Node>& l_Nodes = m_Map[p_Category];
	auto l_Itr = std::find_if(l_Nodes.begin(), l_Nodes.end(), [&p_Label](const Node& p_Node)
	{
		return p_Label == p_Node.m_Label;
	});

	if (l_Itr != l_Nodes.end())
	{
		l_Itr->m_Value = p_Value;
	}
	else
	{
		l_Nodes.push_back(Node(p_Label, p_Value));
	}
}

void DebugTextSubModule::RemoveValue(const std::string& p_Label, const std::string& p_Category)
{
	auto l_Itr = m_Map.find(p_Category);
	if (l_Itr != m_Map.cend())
	{
		auto l_Itr2 = std::find_if(l_Itr->second.cbegin(), l_Itr->second.cend(), [&p_Label](const Node& p_Node)
		{
			return p_Label == p_Node.m_Label;
		});

		if (l_Itr2 != l_Itr->second.cend())
		{
			l_Itr->second.erase(l_Itr2);
		}
		else
		{
			std::cout << "Failed to find label " << p_Label << " in category " << p_Category << ".\n\n";
		}
	}
	else
	{
		std::cout << "Failed to find category: " << p_Category << ".\n\n";
	}
}

void DebugTextSubModule::CmdSetActiveCategory(const std::string& p_Category)
{
	m_ActiveCategory = p_Category;
}

void DebugTextSubModule::CmdList()
{
	CONSOLE_WRITE_SCOPE();
	for (const auto& l_Cat : m_Map)
	{
		std::cout << l_Cat.first << (l_Cat.first == m_ActiveCategory ? " *" : "") << "\n";
		for (const auto& l_Node : l_Cat.second)
		{
			std::cout << "\t" << l_Node.m_Label << ": " << l_Node.m_Value << "\n";
		}
		std::cout << "\n";
	}
}

}
}