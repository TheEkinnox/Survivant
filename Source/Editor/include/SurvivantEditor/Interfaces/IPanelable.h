//IPanelable.h
#pragma once

namespace SvEditor::Interfaces
{
	class IPanelable
	{
	public:
		static constexpr float X_OFFSET = 20.f;

		virtual ~IPanelable() = default;
		virtual void DisplayAndUpdatePanel() = 0;
	};
}
