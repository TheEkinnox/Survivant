#pragma once

#include <SurvivantEditor/MenuItems/PopupMenu.h>
#include "SurvivantCore/Utility/Utility.h"

#include <string>

namespace SvEditor::Interfaces
{
	class ISelectable
	{
	public:
		virtual ~ISelectable() = default;

		struct SelectablePrioKey
		{
			std::string m_name = "";
			size_t m_priority = 0;

			bool operator <(const SelectablePrioKey& p_rhs) const
			{
				if (this->m_priority != p_rhs.m_priority)
					return this->m_priority > p_rhs.m_priority;

				int val = SvCore::Utility::CompareAlphabeticly(this->m_name, p_rhs.m_name);

				if (val == -1)
					return true;

				if (val == 1)
					return false;

				//if equal check case sensitivity
				return this->m_name < p_rhs.m_name;
			}
		};

		virtual const std::string&	GetIcon() = 0;
		virtual const std::string&	GetName() = 0;
		virtual bool				InvokeOpen() = 0;
		virtual bool				InvokeSelected() = 0;
		virtual void				DisplayAndUpdatePopupMenu() = 0;
		virtual bool				GetSelectedState() = 0;
		virtual void				SetSelectedState(bool p_isSelected) = 0;
	};
}