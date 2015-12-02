#pragma once

#include "Hawk/Events/IEvent.h"

struct TestEvent : public Hawk::IEvent
{
	int m_iValue;
	//std::string m_Str;
};

struct TestEvent2 : public Hawk::IEvent
{
	int m_iValue;
	//std::string m_Str;
};