#pragma once
#include "Events/IEvent.h"

namespace Hawk {

struct WindowSizeChangedEvent : IEvent
{
	WindowSizeChangedEvent(UINT32 p_uiWidth, UINT32 p_uiHeight, bool p_bMinimized)
	: m_uiWidth(p_uiWidth)
	, m_uiHeight(p_uiHeight)
	, m_bMinimized(p_bMinimized) {}

	UINT32 m_uiWidth;
	UINT32 m_uiHeight;
	bool m_bMinimized;
};

struct WindowManipulatedEvent : IEvent {};
}