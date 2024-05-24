//GamePanel.cpp

#include "SurvivantEditor/Panels/GamePanel.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/Core/EditorUI.h"
#include "SurvivantApp/Core/WorldContext.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace SvApp::Core;

namespace SvEditor::Panels
{
	GamePanel::GamePanel()
	{
		m_name = NAME; 

		m_buttons.m_buttons.reserve(3);
		m_buttons.m_buttons.push_back(PanelButton(" Start |> ", s_playListenners));
		m_buttons.m_buttons.push_back(PanelButton(" Pause || ", s_pauseListenners));
		m_buttons.m_buttons.push_back(PanelButton(" Frame -> ", s_frameListenners));

		m_world = s_worldCreator({ 0, 0 });
		m_image.SetTexture(m_world->m_renderingContext->GetTextureId(RenderingContext::ETextureType::COLOR));
			
		m_onResize.AddListener([this](const LibMath::Vector2& p_size)
			{
				m_world->m_renderingContext->Resize(p_size);
				m_world->m_renderingContext->Render(m_world->CurrentScene().Get());
				SV_LOG(SvCore::Utility::FormatString("Size = %f, %f", p_size.m_x, p_size.m_y).c_str());
			});
	}

	GamePanel::~GamePanel()
	{
	}

	void GamePanel::SetGameWorldCreator(const WorldContext::WorldCreator& p_worldCreator)
	{
		s_worldCreator = p_worldCreator;
	}

	Panel::ERenderFlags GamePanel::Render()
	{
		static intptr_t tmp = 1;
		static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavInputs;
		bool showWindow = true;
		ERenderFlags flags = ERenderFlags();

		if (m_world->m_isVisalbe = ImGui::Begin(m_name.c_str(), &showWindow, window_flags))
		{
			//focus
			auto val = IsGainedFocus(m_prevFocus);
			if (val == 1 && m_world->m_owningGameInstance)
			{
				m_world->m_isFocused = true;
				m_world->SetInputs();
			}
			else if (val == -1)
			{
				m_world->m_isFocused = false;
				flags = ERenderFlags(flags | DefaultInputs);
			}

			if (IsWindowDifferentSize(m_imageSize))
			{
				m_onResize.Invoke(m_imageSize);
				m_image.SetTexture(m_world->m_renderingContext->GetTextureId(RenderingContext::ETextureType::COLOR));
			}

			m_buttons.DisplayAndUpdatePanel();
			m_image.DisplayAndUpdatePanel();
		}
		ImGui::End();

		if (!showWindow)
			flags = ERenderFlags(flags | ERenderFlags::CLOSE);

		return flags;
	}

	size_t GamePanel::AddPlayListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback)
	{
		return s_playListenners.AddListener(p_callback);
	}

	size_t GamePanel::AddPauseListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback)
	{
		return s_pauseListenners.AddListener(p_callback);

	}

	size_t GamePanel::AddFrameListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback)
	{
		return s_frameListenners.AddListener(p_callback);
	}
}
