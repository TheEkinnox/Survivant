//Panel.h
#pragma once

#include <vector>
#include <string>

namespace SvUI::Panels
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

		std::string GetUniqueName(const char p_name[], size_t p_count);
		size_t GetUniqueIdInName();

		std::string m_name;
	};
}
