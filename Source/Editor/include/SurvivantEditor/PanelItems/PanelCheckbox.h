#pragma once
#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include <string>

namespace SvEditor::PanelItems
{
    class PanelCheckbox final : public PanelInputBase<bool>
    {
    public:
        PanelCheckbox(const std::string& p_name, Value& p_value, const Callback& p_callback = Callback());
        PanelCheckbox(std::string p_name, const GetRefFunc& p_refFunc, const Callback& p_callback = Callback());
        PanelCheckbox(std::string p_name, const GetCopyFunc& p_copyFunc, const Callback& p_callback);
        ~PanelCheckbox() override = default;

        void DisplayAndUpdatePanel() override;

    private:
        std::string m_name;
    };
}
