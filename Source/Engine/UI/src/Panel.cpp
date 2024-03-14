//UI.cpp

#include "SurvivantUI/Panel.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "imgui_internal.h"

#include <stdexcept>

const std::string& UI::Panel::GetName()
{
    return m_name;
}

UI::Panel::Panel(const std::string& p_name):
    m_name(p_name)
{}

std::string UI::Panel::GetUniqueName(const char p_name[], size_t p_count)
{
    return (p_name + (p_count > 0 ? "(" + std::to_string(p_count) + ")" : ""));
}

size_t UI::Panel::GetUniqueIdInName()
{
    if (m_name.size() <= 3)
        return 0;

    auto firstParentheses = m_name.rfind('(');

    if (firstParentheses == std::string::npos)
        return 0;

    //ignore first and last parentheses
    auto strNum = m_name.substr(firstParentheses + 1, m_name.size() - 1);

    int num{};
    size_t pos{};

    try
    {
        num = std::stoi(strNum, &pos);
    }
    catch (std::invalid_argument const& /*ex*/)
    {
        return 0;
    }
    catch (std::out_of_range const& /*ex*/)
    {
        return 0;
    }

    return static_cast<size_t>(num);
}
