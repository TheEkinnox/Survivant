#pragma once
#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include <string>

namespace SvEditor::PanelItems
{
    class PanelDoubleInput final : public PanelInputBase<double>
    {
    public:
        PanelDoubleInput(const std::string& p_name, Value& p_value, const Callback& p_callback = Callback());
        PanelDoubleInput(std::string p_name, const GetRefFunc& p_refFunc, const Callback& p_callback = Callback());
        PanelDoubleInput(std::string p_name, const GetCopyFunc& p_copyFunc, const Callback& p_callback);
        ~PanelDoubleInput() override = default;

        void DisplayAndUpdatePanel() override;

    private:
        std::string m_name;
    };
}
