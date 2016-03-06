#include "pch.h"
#include "ConsoleHistory.h"
#include "Util/StringUtil.h"
#include <boost/filesystem.hpp>
#include <fstream>

namespace Hawk {

const std::string ConsoleHistory::c_Filename = "console_history.txt";

ConsoleHistory::ConsoleHistory(UINT32 p_uiMaxSize)
: m_uiMaxSize(p_uiMaxSize)
, m_uiCurrRecord(0)
, m_bStartState(true)
{
	Load();
}

bool ConsoleHistory::Back()
{
	if (m_Cmds.empty()) return false;

	if (m_bStartState)
	{
		m_bStartState = false;
		return true;
	}

	if (m_uiCurrRecord < m_Cmds.size() - 1)
	{
		m_uiCurrRecord++;
		return true;
	}
	return false;
}

bool ConsoleHistory::Forward()
{
	if (m_uiCurrRecord > 0)
	{
		m_uiCurrRecord--;
		return true;
	}
	return false;
}

const std::string& ConsoleHistory::GetCurrRecord() const
{
	ASSERT(!m_bStartState, "Should not be in start state when calling GetCurrRecord()")
	ASSERT(m_uiCurrRecord < m_Cmds.size(), "CurrRecord out of bounds");
	return m_Cmds[m_uiCurrRecord];
}

void ConsoleHistory::Add(const std::string& p_Cmd)
{
	ASSERT(m_Cmds.size() <= m_uiMaxSize, "MaxSize exceeded");
	if (m_Cmds.empty() || !StringUtil::AreEqual(p_Cmd, m_Cmds.front()))
	{
		m_Cmds.push_front(p_Cmd);
	}

	if (m_Cmds.size() > m_uiMaxSize)
	{
		m_Cmds.erase(m_Cmds.end() - 1);
	}
	m_uiCurrRecord = 0;
	m_bStartState = true;
	Save();
}

void ConsoleHistory::Print() const
{
	for (const std::string& l_Cmd : m_Cmds)
	{
		std::cout << l_Cmd << "\n";
	}
	std::cout << "\n" << m_Cmds.size() << " commands in history. Max: " << m_uiMaxSize << "\n\n";
}

void ConsoleHistory::Clear()
{
	m_Cmds.clear();
	Save();
}

void ConsoleHistory::Load()
{
	std::ifstream l_File(c_Filename);
	if (l_File.is_open())
	{
		std::string l_Line;
		while (std::getline(l_File, l_Line))
		{
			m_Cmds.push_back(l_Line);
		}
		if (m_Cmds.size() > m_uiMaxSize)
		{
			m_Cmds.erase(m_Cmds.begin() + (m_uiMaxSize - 1), m_Cmds.end() - 1);
			LOG("History file contained " << m_Cmds.size() << " commands, was truncated to " << m_Cmds.size(), "console", Warning);
		}
		LOG("Read " << m_Cmds.size() << " commands from " << c_Filename, "console", Debug);
	}
	else
	{
		LOG("Could not open " << c_Filename, "console", Debug);
	}
}

void ConsoleHistory::Save()
{
	std::ofstream l_File(c_Filename);
	if (l_File.is_open())
	{
		for (const std::string& l_Cmd : m_Cmds)
		{
			l_File << l_Cmd << "\n";
		}
	}
	else
	{
		LOG("Could not open " << c_Filename, "console", Error);
	}
}
}