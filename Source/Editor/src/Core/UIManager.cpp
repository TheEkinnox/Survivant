//UIManager.cpp

#include "SurvivantEditor/Core/IUI.h"

#include <SurvivantCore/Debug/Assertion.h>

namespace SvEditor::Core
{
    IUI* IUI::GetCurrentUI()
    {
        ASSERT(m_currentUI != nullptr, "There is no current UI");
        return m_currentUI;
    }
}
