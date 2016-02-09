#pragma once
#include "Events/IEvent.h"

namespace Hawk {

struct WindowSizeChangedEvent : IEvent
{
	WindowSizeChangedEvent(unsigned int p_uiWidth, unsigned int p_uiHeight, bool p_bMinimized)
	: m_uiWidth(p_uiWidth)
	, m_uiHeight(p_uiHeight)
	, m_bMinimized(p_bMinimized) {}

	unsigned int m_uiWidth;
	unsigned int m_uiHeight;
	bool m_bMinimized;
};

struct WindowManipulatedEvent : IEvent {};
}