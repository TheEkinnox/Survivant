//Panel.h
#pragma once

#include "Vector/Vector2.h"

#include <vector>
#include <string>

namespace SvEditor::Panels
{
	class Panel
	{
	public:
		enum ERenderFlags
		{
			CLOSE =			1 << 0,
			DefaultInputs =	1 << 1
		};

		const std::string& GetName();
		virtual ERenderFlags Render() = 0;

	protected:
		Panel() = default;
		Panel(const std::string& p_name);

		static bool IsWindowDifferentSize(LibMath::Vector2& p_oldSize, bool& p_lastVal);

		/// <summary>
		/// Check if window gained or lost focus
		/// </summary>
		/// <param name="p_prevFocus">previous focus value</param>
		/// <returns>0 if same focus, 1 if gained or -1 if lost</returns>
		static int IsGainedFocus(bool& p_currentFocus);

		std::string GetUniqueName(const char p_name[], size_t p_count);
		size_t GetUniqueIdInName();


		std::string m_name;
	};
}
