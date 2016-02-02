#pragma once

#include "Events/IEvent.h"
#include <string>

namespace Hawk {
namespace Gfx {

struct UpdateDebugTextEvent : public IEvent
{
	UpdateDebugTextEvent(std::string&& p_Label, std::string&& p_Value, std::string&& p_Category)
	: m_Label(std::move(p_Label))
	, m_Value(std::move(p_Value))
	, m_Category(std::move(p_Category)) {}

	std::string m_Label;
	std::string m_Value;
	std::string m_Category;
};

struct RemoveDebugTextEvent : public IEvent
{
	RemoveDebugTextEvent(std::string&& p_Label)
	: m_Label(std::move(p_Label)) {}

	std::string m_Label;
	std::string m_Category;
};

}}