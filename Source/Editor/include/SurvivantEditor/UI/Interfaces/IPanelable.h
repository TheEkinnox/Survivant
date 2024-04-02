//IPanelable.h
#pragma once

#include <string>

namespace SvEditor::UI::Interfaces
{
	class IPanelable
	{
	public:
		virtual ~IPanelable() = default;
		virtual void DisplayAndUpdatePanel() = 0;
	};
}
