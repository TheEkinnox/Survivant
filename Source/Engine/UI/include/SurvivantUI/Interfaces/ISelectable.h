#pragma once

#include <SurvivantUI/MenuItems/PopupMenu.h>

#include <string>

namespace SvUI::Interfaces
{
	class ISelectable
	{
	public:
		virtual ~ISelectable() = default;

		virtual const std::string&	GetIcon() = 0;
		virtual const std::string&	GetName() = 0;
		virtual bool				InvokeDoubleClick() = 0;
		virtual void				DisplayAndUpdatePopupMenu() = 0;
		virtual bool				GetSelectedState() = 0;
		virtual void				SetSelectedState(bool p_isSelected) = 0;
	};
}