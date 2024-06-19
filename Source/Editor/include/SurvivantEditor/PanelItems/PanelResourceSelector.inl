//PanelResourceSelector.inl
#pragma once

#include "SurvivantEditor/Core/EditorUI.h"
#include "SurvivantEditor/MenuItems/MenuButton.h"
#include "SurvivantEditor/PanelItems/PanelResourceSelector.h"
#include "SurvivantEditor/Panels/ContentDrawerPanel.h"

#include <SurvivantCore/Resources/ResourceManager.h>
#include <SurvivantCore/Resources/ResourceRegistry.h>
#include <SurvivantCore/Utility/ServiceLocator.h>

#include <imgui.h>

namespace SvEditor::PanelItems
{
    template <class T, class RefT>
    PanelResourceSelector<T, RefT>::PanelResourceSelector(
        const std::string& p_name, const Ref& p_resourceRef, const Callback& p_callback)
        : PanelResourceSelector(p_name, p_resourceRef, true, p_callback)
    {
    }

    template <class T, class RefT>
    PanelResourceSelector<T, RefT>::PanelResourceSelector(
        const std::string& p_name, const Ref& p_resourceRef, bool p_displayResource, const Callback& p_callback)
        : PanelInputBase<RefT>(p_resourceRef, p_callback)
    {
        m_name            = p_name;
        m_displayResource = p_displayResource;

        m_allResources = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
            m_name,
            [this]()
            {
                this->FetchAllResources();
            },
            [this]()
            {
                this->m_allResources->m_items.clear();
            }
        ));
    }

    template <class T, class RefT>
    PanelResourceSelector<T, RefT>::PanelResourceSelector(const PanelResourceSelector& p_other)
        : PanelInputBase<RefT>({})
    {
        *this = p_other;
    }

    template <class T, class RefT>
    PanelResourceSelector<T, RefT>::PanelResourceSelector(PanelResourceSelector&& p_other) noexcept
        : PanelInputBase<RefT>({})
    {
        *this = std::move(p_other);
    }

    template <class T, class RefT>
    PanelResourceSelector<T, RefT>& PanelResourceSelector<T, RefT>::operator=(const PanelResourceSelector& p_other)
    {
        if (this == &p_other)
            return *this;

        this->m_getRef          = p_other.m_getRef;
        this->m_callback        = p_other.m_callback;
        this->m_name            = p_other.m_name;
        this->m_displayResource = p_other.m_displayResource;

        m_allResources = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
            m_name,
            [this]()
            {
                this->FetchAllResources();
            },
            [this]()
            {
                this->m_allResources->m_items.clear();
            }
        ));

        return *this;
    }

    template <class T, class RefT>
    PanelResourceSelector<T, RefT>& PanelResourceSelector<T, RefT>::operator=(PanelResourceSelector&& p_other) noexcept
    {
        if (this == &p_other)
            return *this;

        this->m_getRef          = std::move(p_other.m_getRef);
        this->m_callback        = std::move(p_other.m_callback);
        this->m_name            = std::move(p_other.m_name);
        this->m_displayResource = std::move(p_other.m_displayResource);

        m_allResources = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
            m_name,
            [this]()
            {
                this->FetchAllResources();
            },
            [this]()
            {
                this->m_allResources->m_items.clear();
            }
        ));

        return *this;
    }

    template <class T, class RefT>
    void PanelResourceSelector<T, RefT>::DisplayAndUpdatePanel()
    {
        static auto flag = ImGuiInputTextFlags_ReadOnly;

        m_allResources->DisplayAndUpdatePanel();

        if (m_displayResource)
        {
            ImGui::SameLine();
            std::string resourceName = this->GetRef().GetPath();
            resourceName             = resourceName.empty() ? "(none)" : resourceName;
            ImGui::PushID(resourceName.c_str());
            PanelInputBase<RefT>::DisplayAndUpdatePanel();
            ImGui::InputText("##", &resourceName[0], resourceName.size(), flag);
            ImGui::PopID();
        }
    }

    template <class T, class RefT>
    void PanelResourceSelector<T, RefT>::FetchAllResources()
    {
        using namespace SvCore::Resources;
        using namespace SvEditor::Panels;

        const auto& rr = ResourceRegistry::GetInstance();

        auto all = ContentDrawerPanel::GetAllFilePaths(rr.GetRegisteredTypeName<T>());

        m_allResources->m_items.clear();
        m_allResources->m_items.reserve(all.size() + 1);

        m_allResources->m_items.emplace_back(std::make_unique<MenuButton>(
            "(default)", [this](char) mutable
            {
                auto& ref = (this->GetRef() = {});
                if (this->m_callback)
                    this->m_callback(ref);
            }
        ));

        auto& ref = this->GetRef();

        const ResourceManager& resourceManager = SV_SERVICE(ResourceManager);
        const std::string&     currentPath     = resourceManager.GetFullPath(ref.GetPath());

        bool foundPath = false;

        if constexpr (!std::is_same_v<IResource, T>)
        {
            for (const std::string& resourcePath : all)
            {
                if (!currentPath.empty() && resourceManager.GetFullPath(resourcePath) == currentPath)
                    foundPath = true;

                m_allResources->m_items.emplace_back(std::make_unique<MenuButton>(
                    resourcePath, [this, resourcePath](char) mutable
                    {
                        const RefT& resource = (this->GetRef() = ResourceRef<T>(resourcePath));

                        if (!SV_SERVICE(ResourceManager).Get<T>(resourcePath))
                            reinterpret_cast<Core::EditorUI*>(SV_CURRENT_UI())->SpawnLoadingPanel();

                        if (this->m_callback)
                            this->m_callback(resource);
                    }
                ));
            }
        }

        if (!currentPath.empty() && !foundPath)
            ref = {};
    }
}
