//GamePanel.cpp

#include "SurvivantEditor/Panels/GamePanel.h"

#include "SurvivantEditor/Core/EditorEngine.h"
#include "SurvivantEditor/PanelItems/PanelConditionalDisplay.h"

#include <imgui.h>

using namespace SvApp;
using namespace SvApp::Core;

namespace SvEditor::Panels
{
	GamePanel::GamePanel() : m_prevFocus(false)
	{
		m_name = NAME;

		m_buttons[0] = std::make_unique<PanelConditionalDisplay>(
			std::make_unique<PanelButton>(" Stop X ", s_playListeners),
			std::make_unique<PanelButton>(" Start |> ", s_playListeners),
			PanelConditionalDisplay::GetStateFunc([]
			{
				return dynamic_cast<const Core::EditorEngine&>(*SV_ENGINE()).IsPlayInEditor();
			})
		);

		m_buttons[1] = std::make_unique<PanelConditionalDisplay>(
			std::make_unique<PanelButton>(" Resume |> ", s_pauseListeners),
			std::make_unique<PanelButton>(" Pause || ", s_pauseListeners),
			PanelConditionalDisplay::GetStateFunc([]
			{
				return dynamic_cast<const Core::EditorEngine&>(*SV_ENGINE()).IsPaused();
			})
		);

		m_buttons[2] = std::make_unique<PanelButton>(" Frame -> ", s_frameListeners);

		m_world = s_worldCreator({ 0, 0 });
		m_prevFocus = false;
		m_prevCursorMode = InputManager::GetInstance().GetCursorMode();

		m_world->m_inputs = std::make_shared<InputManager::InputBindings>();

		auto& keyCallbacks = m_world->m_inputs->m_keyCallbacks;

		static const auto escKeyType = InputManager::KeyboardKeyType(EKey::ESCAPE, EKeyState::PRESSED, EInputModifier());
		keyCallbacks[escKeyType] = [this](char)
		{
			ImGui::SetWindowFocus(nullptr);
		};

		m_image.SetTexture(m_world->m_renderingContext->GetTextureId(RenderingContext::ETextureType::COLOR));

		m_onResize.AddListener([this](const LibMath::Vector2& p_size)
			{
				m_world->m_renderingContext->Resize(p_size);
				m_world->m_renderingContext->Render(m_world->CurrentScene().Get());
				SV_LOG(SvCore::Utility::FormatString("Size = %f, %f", p_size.m_x, p_size.m_y).c_str());
			});
	}

	void GamePanel::SetGameWorldCreator(const WorldContext::WorldCreator& p_worldCreator)
	{
		s_worldCreator = p_worldCreator;
	}

	Panel::ERenderFlags GamePanel::Render()
	{
		static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavInputs;

		bool showWindow = true;
		ERenderFlags flags = ERenderFlags();

		if ((m_world->m_isVisalbe = ImGui::Begin(m_name.c_str(), &showWindow, windowFlags)))
		{
			//focus
			auto val = IsGainedFocus(m_prevFocus);
			if (val == 1)
			{
				m_world->m_isFocused = true;
				if (m_world->m_owningGameInstance)
				{
					m_world->SetInputs();
					InputManager::GetInstance().SetCursorMode(m_prevCursorMode);
				}

				m_world->m_renderingContext->Render(m_world->CurrentScene().Get());
			}
			else if (val == -1)
			{
				m_world->m_isFocused = false;

				if (m_world->m_owningGameInstance)
				{
					flags = static_cast<ERenderFlags>(flags | DefaultInputs);

					InputManager& inputManager = InputManager::GetInstance();
					m_prevCursorMode = inputManager.GetCursorMode();
					inputManager.SetCursorMode(ECursorMode::NORMAL);
				}
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

	size_t GamePanel::AddPlayListener(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback)
	{
		return s_playListeners.AddListener(p_callback);
	}

	size_t GamePanel::AddPauseListener(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback)
	{
		return s_pauseListeners.AddListener(p_callback);

	}

	size_t GamePanel::AddFrameListener(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback)
	{
		return s_frameListeners.AddListener(p_callback);
	}
}
