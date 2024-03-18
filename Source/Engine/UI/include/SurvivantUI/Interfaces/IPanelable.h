//IPanelable.h
#pragma once

#include <string>

namespace SvUI::Interfaces
{
	class IPanelable
	{
	public:
		virtual ~IPanelable() = default;
		virtual void DisplayAndUpdatePanel() = 0;
	};
}
