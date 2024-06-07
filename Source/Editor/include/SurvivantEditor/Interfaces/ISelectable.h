#pragma once

#include "SurvivantCore/Events/Event.h"
#include "SurvivantCore/Utility/Utility.h"
#include "SurvivantEditor/Interfaces/IInspectorable.h"
#include <SurvivantEditor/MenuItems/PopupMenu.h>

#include <string>

namespace SvEditor::Interfaces
{
	class ISelectable : public IInspectorable
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

				int val = SvCore::Utility::CompareAlphabetically(this->m_name, p_rhs.m_name);

				if (val == -1)
					return true;

				if (val == 1)
					return false;

				//if equal check case sensitivity
				return this->m_name < p_rhs.m_name;
			}
		};

		virtual const std::string&	GetIcon()override = 0;
		virtual const std::string&	GetName()override = 0;
		virtual bool				Open() { s_onOpened.Invoke(this); return false; };
		virtual bool				Select() { SetSelectedState(true); s_onSelected.Invoke(this); return false; };
		virtual void				DisplayAndUpdatePopupMenu() = 0;
		virtual bool				GetSelectedState() = 0;

		void ClearSelection() {

			auto prevState = GetSelectedState();
			SetSelectedState(false);

			if (prevState)
				s_onClearSelected.Invoke();
		};

		bool ToggleSelection()
		{
			if (GetSelectedState())
				ClearSelection();
			else
				Select();

			return GetSelectedState();
		}

		inline static SvCore::Events::Event<ISelectable*>	s_onOpened;
		inline static SvCore::Events::Event<ISelectable*>	s_onSelected;
		inline static SvCore::Events::Event<>				s_onClearSelected;

	protected:
		virtual void				SetSelectedState(bool p_isSelected) = 0;
	};
}