#pragma once

#include "Hawk/Events/IEvent.h"

struct TestEvent : public Hawk::IEvent
{
	int32_t m_iValue;
	//std::string m_Str;
};

struct TestEvent2 : public Hawk::IEvent
{
	int32_t m_iValue;
	//std::string m_Str;
};

struct TestEvent3 : public Hawk::IEvent
{
	std::string m_Str;
};