//ITextable.h
#pragma once

#include "SurvivantEditor/UI/Interfaces/IPanelable.h"

namespace SvEditor::UI::Interfaces
{
	class ITextable : public IPanelable
	{
	public:
		virtual ~ITextable() = default;

		virtual void DisplayAndUpdatePanel() override = 0;

		/// <summary>
		/// Get text as string
		/// </summary>
		/// <param name="p_len">Length of string, if len = 0 : return whole string</param>
		/// <returns></returns>
		virtual std::string GetString(size_t p_len = 0)const = 0;

		virtual size_t GetLength()const = 0;
	};
}