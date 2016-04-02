#include "pch.h"

#ifdef HAWK_DEBUG
#include "ConsoleInputParser.h"
#include "Util/StringUtil.h"
#include <boost/tokenizer.hpp>

namespace Hawk {

ConsoleInputParser::ConsoleInputParser(const std::string& p_Input)
{
	using Separator = boost::char_separator<char>;
	Separator l_Sep(" ");
	boost::tokenizer<Separator> l_Tokenizer(p_Input, l_Sep);
	const int64 l_iNumTokens = std::distance(l_Tokenizer.begin(), l_Tokenizer.end());
	THROW_IF_NOT(l_iNumTokens > 0, "Invalid number of input tokens for console command. Num=" << l_iNumTokens);

	auto l_Itr = l_Tokenizer.begin();
	m_Command = StringUtil::ToLower(*l_Itr);
	l_Itr++;

	for (; l_Itr != l_Tokenizer.end(); l_Itr++)
	{
		m_Args.push_back(*l_Itr);
	}
}

void ConsoleInputParser::TryExtendArgs(std::size_t p_NumArgs)
{
	while (m_Args.size() < p_NumArgs)
	{
		m_Args.push_back(std::string());
	}
}
}
#endif