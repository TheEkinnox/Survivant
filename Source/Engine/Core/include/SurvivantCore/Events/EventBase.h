//EventBase.h
#pragma once

namespace SvCore::Events
{
	class EventBase
	{
	public:

	public:
		EventBase() = default;
		~EventBase() = default;

	public:
		virtual void ClearListeners() = 0;
	};
}