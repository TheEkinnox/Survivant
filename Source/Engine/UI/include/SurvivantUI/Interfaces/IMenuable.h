//IMenuable.h
#pragma once

namespace SvUI::Interfaces
{
	class IMenuable
	{
	public:
		virtual ~IMenuable() = default;
		virtual IMenuable* Clone() const = 0;
		virtual void DisplayAndUpdateMenu() = 0;
	};	
}
