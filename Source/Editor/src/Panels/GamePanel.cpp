//GamePanel.cpp

#include "SurvivantEditor/Panels/GamePanel.h"

#include "SurvivantEditor/Core/EditorEngine.h"
#include "SurvivantEditor/PanelItems/PanelConditionalDisplay.h"

#include <imgui.h>

using namespace SvApp::Core;

namespace SvEditor::Panels
{
	GamePanel::GamePanel() : m_prevFocus(false)
	{
		m_name = NAME;

		m_buttons[0] = std::make_unique<PanelConditionalDisplay>(
			std::make_unique<PanelButton>(" Stop X ", s_playListenners),
			std::make_unique<PanelButton>(" Start |> ", s_playListenners),
			PanelConditionalDisplay::GetStateFunc([]
			{
				return dynamic_cast<const Core::EditorEngine&>(*SV_ENGINE()).IsPlayInEditor();
			})
		);

		m_buttons[1] = std::make_unique<PanelConditionalDisplay>(
			std::make_unique<PanelButton>(" Resume |> ", s_pauseListenners),
			std::make_unique<PanelButton>(" Pause || ", s_pauseListenners),
			PanelConditionalDisplay::GetStateFunc([]
			{
				return dynamic_cast<const Core::EditorEngine&>(*SV_ENGINE()).IsPaused();
			})
		);

		m_buttons[2] = std::make_unique<PanelButton>(" Frame -> ", s_frameListenners);

		m_world = s_worldCreator({ 0, 0 });
		m_prevFocus = false;

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

		if ((m_world->m_isVisalbe = ImGui::Begin(m_name.c_str(), &showWindow, window_flags)))
		{
			//focus
			auto val = IsGainedFocus(m_prevFocus);
			if (val == 1)
			{
				m_world->m_isFocused = true;
				if (m_world->m_owningGameInstance)
					m_world->SetInputs();
			}
			else if (val == -1)
			{
				m_world->m_isFocused = false;
				if (m_world->m_owningGameInstance)
					flags = ERenderFlags(flags | DefaultInputs);
			}

			if (IsWindowDifferentSize(m_imageSize))
				m_onResize.Invoke(m_imageSize);

			for (uint8_t i = 0; i < 3; ++i)
			{
				if (i != 0)
					ImGui::SameLine();

				m_buttons[i]->DisplayAndUpdatePanel();
			}

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
