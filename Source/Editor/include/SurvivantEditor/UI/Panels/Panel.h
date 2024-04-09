//Panel.h
#pragma once

#include "Vector/Vector2.h"

#include <vector>
#include <string>

namespace SvEditor::UI::Panels
{
	class Panel
	{
	public:
		enum ERenderFlags
		{
			CLOSE =					1 << 0,
		};

		const std::string& GetName();
		virtual ERenderFlags Render() = 0;

	protected:
		Panel() = default;
		Panel(const std::string& p_name);

		static bool IsWindowDifferentSize(LibMath::Vector2& p_oldSize, bool& p_lastVal);

		std::string GetUniqueName(const char p_name[], size_t p_count);
		size_t GetUniqueIdInName();

		std::string m_name;
	};
}
