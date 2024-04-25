#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include <string>

namespace SvEditor::Interfaces
{
	class IInspectorable : public IPanelable
	{
	public:
		virtual ~IInspectorable() = default;

		virtual const std::string&	GetIcon() = 0;
		virtual const std::string&	GetName() = 0;

		virtual void DisplayAndUpdatePanel()override = 0;
	};
}