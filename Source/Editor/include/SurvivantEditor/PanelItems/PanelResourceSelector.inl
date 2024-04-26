//PanelResourceSelector.inl
#pragma once

#include "SurvivantEditor/PanelItems/PanelResourceSelector.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
	template<class T>
	inline PanelResourceSelector<T>::PanelResourceSelector(
		const std::string& p_name, Ref p_resourceRef, const Callback& p_callback) :
		PanelInputBase(GetRefFunc([r = p_resourceRef]() mutable -> Ref& { return r; }), p_callback)
	{
	}

	template<class T>
	inline void PanelResourceSelector<T>::DisplayAndUpdatePanel()
	{
		ImGui::Text(m_name.c_str());
		ImGui::SameLine();

		ImGui::PushID(m_name.c_str());
		if (ImGui::Combo("##", &val, m_items.c_str()) && m_callback)
			m_callback(val);

		ImGui::PopID();

	}

	template<class T>
	inline void PanelResourceSelector<T>::UpdatePopup()
	{
		m_allResources.m_items.clear();


	}
}