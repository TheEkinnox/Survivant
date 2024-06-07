#include "SurvivantEditor/MenuItems/MenuButton.h"
#include "SurvivantEditor/PanelItems/PanelGenericResourceSelector.h"
#include "SurvivantEditor/Panels/ContentDrawerPanel.h"

#include <SurvivantCore/Resources/ResourceRegistry.h>
#include <SurvivantCore/Resources/ResourceManager.h>

using namespace SvCore::Resources;
using namespace SvEditor::Panels;

namespace SvEditor::PanelItems
{
    PanelGenericResourceSelector::PanelGenericResourceSelector(
        const std::string& p_name, const Ref& p_resourceRef, const Callback& p_callback)
        : PanelGenericResourceSelector(p_name, p_resourceRef, true, p_callback)
    {
    }

    PanelGenericResourceSelector::PanelGenericResourceSelector(
        std::string p_name, const Ref& p_resourceRef, const bool p_displayResource, const Callback& p_callback)
        : PanelResourceSelector("resource ", p_resourceRef, p_displayResource, p_callback), m_label(std::move(p_name))
    {
        FetchResourceTypes();

        m_resourceTypeSelection = std::make_shared<PanelUniqueSelection>(
            "type     ",
            m_resourceTypes,
            m_currentType,
            [this](const int)
            {
                FetchTypedResources();
            }
        );
    }

    PanelGenericResourceSelector::PanelGenericResourceSelector(const PanelGenericResourceSelector& p_other)
        : PanelResourceSelector(p_other)
    {
        *this = p_other;
    }

    PanelGenericResourceSelector::PanelGenericResourceSelector(PanelGenericResourceSelector&& p_other) noexcept
        : PanelResourceSelector("", {}, {})
    {
        *this = std::move(p_other);
    }

    void PanelGenericResourceSelector::DisplayAndUpdatePanel()
    {
        static constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen |
            ImGuiTreeNodeFlags_NoAutoOpenOnLog;

        if (ImGui::TreeNodeEx(m_label.c_str(), flags))
        {
            ImGui::PushID(m_label.c_str());
            ImGui::Dummy({ X_OFFSET, 0.f });
            ImGui::SameLine();
            ImGui::BeginGroup();

            m_resourceTypeSelection->DisplayAndUpdatePanel();
            PanelResourceSelector::DisplayAndUpdatePanel();

            ImGui::EndGroup();
            ImGui::PopID();
        }
    }

    void PanelGenericResourceSelector::FetchResourceTypes()
    {
        m_resourceTypes.clear();
        m_resourceTypes.emplace_back("(any)");

        const auto types = ResourceRegistry::GetInstance().GetRegisteredNames();
        m_resourceTypes.insert(m_resourceTypes.end(), types.begin(), types.end());

        auto& resource = GetRef();
        m_currentType  = 0;

        if (!resource.GetType().empty())
        {
            const auto it = std::ranges::find(m_resourceTypes, resource.GetType());

            if (it != m_resourceTypes.end())
                m_currentType = static_cast<int>(it - m_resourceTypes.begin());
            else
                resource = {};
        }
    }

    void PanelGenericResourceSelector::FetchAllResources()
    {
        if (m_currentType != 0)
            return FetchTypedResources();

        m_allResources->m_items.clear();
        m_allResources->m_items.emplace_back(std::make_unique<MenuButton>(
            "(none)", [this](char) mutable
            {
                GetRef() = {};
                if (m_callback)
                    m_callback({});
            }
        ));

        const ResourceManager& resourceManager = ResourceManager::GetInstance();

        auto& ref = GetRef();

        const std::string& currentPath = resourceManager.GetFullPath(ref.GetPath());

        bool foundPath = false;

        for (auto& [typeName, branches] : ContentDrawerPanel::GetAllFilePaths())
        {
            for (const auto& branch : branches)
            {
                const std::string& resourcePath = branch.lock()->GetPath();

                if (!currentPath.empty() && resourceManager.GetFullPath(resourcePath) == currentPath)
                    foundPath = true;

                m_allResources->m_items.emplace_back(std::make_unique<MenuButton>(
                    resourcePath, [this, typeName, resourcePath](char) mutable
                    {
                        const auto it = std::ranges::find(m_resourceTypes, typeName);
                        m_currentType = static_cast<int>(it - m_resourceTypes.begin());

                        const GenericResourceRef resource = { typeName, resourcePath };
                        GetRef()                          = resource;

                        if (m_callback)
                            m_callback(resource);
                    }
                ));
            }
        }

        if (!currentPath.empty() && !foundPath)
            ref = { ref.GetType(), "" };
    }

    void PanelGenericResourceSelector::FetchTypedResources()
    {
        if (m_currentType == 0)
            return FetchAllResources();

        const std::string typeName = m_resourceTypes[m_currentType];
        const auto        resPaths = ContentDrawerPanel::GetAllFilePaths(typeName);

        m_allResources->m_items.clear();
        m_allResources->m_items.reserve(resPaths.size() + 1);

        m_allResources->m_items.emplace_back(std::make_unique<MenuButton>(
            "(default)", [this](char) mutable
            {
                GetRef() = {};
                if (m_callback)
                    m_callback({});
            }
        ));

        auto&              ref         = GetRef();
        const std::string& currentType = ref.GetType();

        if ((!currentType.empty() && currentType != typeName))
            ref = {};

        const ResourceManager& resourceManager = ResourceManager::GetInstance();
        const std::string&     currentPath     = resourceManager.GetFullPath(ref.GetPath());

        bool foundPath = false;

        for (const std::string& resourcePath : resPaths)
        {
            if (!currentPath.empty() && resourceManager.GetFullPath(resourcePath) == currentPath)
                foundPath = true;

            m_allResources->m_items.emplace_back(std::make_unique<MenuButton>(
                resourcePath, [this, resourcePath](char) mutable
                {
                    const std::string         type     = m_resourceTypes[m_currentType];
                    const GenericResourceRef& resource = (GetRef() = { type, resourcePath });

                    if (m_callback)
                        m_callback(resource);
                }
            ));
        }

        if (!currentPath.empty() && !foundPath)
            ref = { currentType, "" };
    }
}
