//UIManager.cpp

#include "SurvivantEditor/Core/IUI.h"

#include "Arithmetic.h"
#include "SurvivantCore\Debug\Assertion.h"

namespace SvEditor::Core
{

	IUI* SvEditor::Core::IUI::GetCurrentUI()
	{
		//if not null, has UI
		ASSERT(m_currentUI != nullptr, "There is no current UI");

		return m_currentUI;
	}

	LibMath::Vector4 IUI::GetColorFromString(const std::string& p_string)
	{
		static std::hash<std::string> s_hash;

		auto val = s_hash(p_string.substr(0));

		return size_tToRGBA(val);
	}

	LibMath::Vector4 IUI::size_tToRGBA(size_t p_value)
	{
		//modified from
		//credit to https://stackoverflow.com/a/2262117/2737978 for the idea of how to implement

		using namespace LibMath;

		size_t blue = LibMath::floor(p_value % 256);
		size_t green = LibMath::floor(p_value / 256 % 256);
		size_t red = LibMath::floor(p_value / 256 / 256 % 256);

		//the color is /2 so its never too dark
		return LibMath::Vector4(
			1.0f - (static_cast<float>(red) / 256.0f / 2.0f),
			1.0f - (static_cast<float>(green) / 256.0f / 2.0f),
			1.0f - (static_cast<float>(blue) / 256.0f / 2.0f),
			1);
	}
}