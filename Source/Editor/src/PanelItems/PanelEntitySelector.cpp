//PanelVec3Input.cpp
#include "SurvivantEditor/PanelItems/PanelEntitySelector.h"
#include "SurvivantEditor/PanelItems/PanelConditionalDisplay.h"
#include "SurvivantApp/Core/IEngine.h"

using namespace SvCore::ECS;

namespace SvEditor::PanelItems
{
    PanelEntitySelector::PanelEntitySelector(
        const std::string& p_name, const GetRefFunc& p_resourceRef, const Callback& p_callback)
        : PanelInputBase(p_resourceRef, p_callback),
        m_input(p_name, PanelUInt32Input::GetCopyFunc(
                [this]() -> uint32_t
                {
                    return GetRef().GetEntity().GetIndex();
                }),
            [this](uint32_t p_index)
            {
                auto& handle = GetRef();
                handle       = SV_ENGINE()->GetCurrentScene()->Find(p_index);

                [[unlikely]] if (m_callback)
                    m_callback(handle);
            })
    {
    }

    PanelEntitySelector::PanelEntitySelector(
        const std::string& p_name, const GetCopyFunc& p_resourceCopy, const Callback& p_callback)
        : PanelInputBase(p_resourceCopy, p_callback),
        m_input(p_name, PanelUInt32Input::GetCopyFunc(
                [this]() -> uint32_t
                {
                    return GetRef().GetEntity().GetIndex();
                }),
            [this](uint32_t p_index)
            {
                if (m_callback)
                    m_callback(SV_ENGINE()->GetCurrentScene()->Find(p_index));
            })
    {
    }

    PanelEntitySelector::PanelEntitySelector(const PanelEntitySelector& p_other)
        : PanelInputBase(p_other.m_getRef, p_other.m_callback),
        m_input(p_other.m_input.m_name, PanelUInt32Input::GetCopyFunc(
                [this]() -> uint32_t
                {
                    return GetRef().GetEntity().GetIndex();
                }),
            [this](uint32_t p_index)
            {
                auto& handle = GetRef();
                handle       = SV_ENGINE()->GetCurrentScene()->Find(p_index);

                [[unlikely]] if (m_callback)
                    m_callback(handle);
            })
    {
        *this = p_other;
    }

    PanelEntitySelector::PanelEntitySelector(PanelEntitySelector&& p_other) noexcept
        : PanelInputBase(std::move(p_other.m_getRef), std::move(p_other.m_callback)),
        m_input(std::move(p_other.m_input.m_name), PanelUInt32Input::GetCopyFunc([this]() -> uint32_t
            {
                return GetRef().GetEntity().GetIndex();
            }),
            [this](uint32_t p_index)
            {
                auto& handle = GetRef();
                handle       = SV_ENGINE()->GetCurrentScene()->Find(p_index);

                [[unlikely]] if (m_callback)
                    m_callback(handle);
            })
    {
    }

    PanelEntitySelector& PanelEntitySelector::operator=(const PanelEntitySelector& p_other)
    {
        m_input = PanelUInt32Input(p_other.m_input.m_name, PanelUInt32Input::GetCopyFunc([this]() -> uint32_t
            {
                return GetRef().GetEntity().GetIndex();
            }),
            [this](uint32_t p_index)
            {
                auto& handle = GetRef();
                handle       = SV_ENGINE()->GetCurrentScene()->Find(p_index);

                [[unlikely]] if (m_callback)
                    m_callback(handle);
            });

        return *this;
    }

    void PanelEntitySelector::DisplayAndUpdatePanel()
    {
        m_input.DisplayAndUpdatePanel();
    }
}
