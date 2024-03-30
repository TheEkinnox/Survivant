//Window.cpp

#include "SurvivantApp/Inputs/InputManager.h"
#include "SurvivantApp/Windows/Window.h"
#include "SurvivantCore/Debug/Assertion.h"

#include <GLFW/glfw3.h>

using namespace SvApp;

void IMKeyCallback(GLFWwindow* /*p_window*/, int p_key, int p_scancode, int p_action, int p_mods)
{
    InputManager::GetInstance().CallInput(
        InputManager::KeyboardKeyType(
            static_cast<EKey>(p_key), 
            static_cast<EKeyState>(p_action), 
            static_cast<EInputModifier>(p_mods)), 
        static_cast<char>(p_scancode));
}  

void IMMouseCallback(GLFWwindow* p_window, int p_button, int p_action, int p_mods)
{
    double xpos, ypos;
    glfwGetCursorPos(p_window, &xpos, &ypos);

    InputManager::GetInstance().CallInput(
        InputManager::MouseKeyType(
            static_cast<EMouseButton>(p_button),
            static_cast<EMouseButtonState>(p_action),
            static_cast<EInputModifier>(p_mods)),
        static_cast<float>(xpos), 
        static_cast<float>(ypos));
}

void WindowCloseCallback(GLFWwindow* /*window*/)
{
    SvCore::Events::EventManager::GetInstance().Invoke<Window::WindowCloseRequest>();
}

void WindowSizeCallback(GLFWwindow* /*window*/, int p_width, int p_height)
{
    SvCore::Events::EventManager::GetInstance().Invoke<Window::OnWindowSize>(p_width, p_height);
}


void WindowFramebufferSizeCallback(GLFWwindow* /*window*/, int p_width, int p_height)
{
    //glViewport (0, 0, p_width, p_height);
    SvCore::Events::EventManager::GetInstance().Invoke<Window::OnFrameBufferSize>(p_width, p_height);
}

void WindowContentScaleCallback(GLFWwindow* /*window*/, float p_xscale, float p_yscale)
{
    SvCore::Events::EventManager::GetInstance().Invoke<Window::OnWindowContentScale>(p_xscale, p_yscale);
}

void WindowMinimizeCallback(GLFWwindow* /*window*/, int p_iconified)
{
    SvCore::Events::EventManager::GetInstance().Invoke<Window::WindowMinimize>(p_iconified != 0);
}

Window::Window(std::string p_name)
{
    //init
    m_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    m_window = glfwCreateWindow(mode->width, mode->height, p_name.c_str(), nullptr, nullptr);
    m_isInFullscreen = false;

    int x;
    glfwGetWindowPos(m_window, &x, &m_yWindowedPos);
    glfwSetWindowPos(m_window, 0, m_yWindowedPos);

    glfwMakeContextCurrent(m_window);

    SetupWindowCallbacks();
    SetupInputManagerCallbacks();
}

SvApp::Window::Window(std::string p_name, int p_width , int p_height, int p_x, int p_y)
{
    //init
    m_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    m_window = glfwCreateWindow(p_width, p_height, p_name.c_str(), nullptr, nullptr);
    m_isInFullscreen = false;

    glfwSetWindowPos(m_window, p_x, p_y);
    glfwMakeContextCurrent(m_window);

    m_yWindowedPos = p_y;

    SetupWindowCallbacks();
    SetupInputManagerCallbacks();
}

SvApp::Window::~Window()
{
    glfwDestroyWindow(m_window);
}

GLFWwindow* SvApp::Window::GetWindow()
{
    return m_window;
}

void SvApp::Window::SetupWindowCallbacks() const
{
    glfwSetWindowCloseCallback(m_window, WindowCloseCallback);
    glfwSetWindowSizeCallback(m_window, WindowSizeCallback);
    glfwSetFramebufferSizeCallback(m_window, WindowFramebufferSizeCallback);
    glfwSetWindowIconifyCallback(m_window, WindowMinimizeCallback);
}

void SvApp::Window::SetupInputManagerCallbacks() const
{
    glfwSetKeyCallback(m_window, IMKeyCallback);
    glfwSetMouseButtonCallback(m_window, IMMouseCallback);
}

void SvApp::Window::GetMousePos(double& p_x, double& p_y) const
{
    glfwGetCursorPos(m_window, &p_x, &p_y);
}

void SvApp::Window::GetWindowSize(int& p_width, int& p_height) const
{
    glfwGetWindowSize(m_window, &p_width, &p_height);
}

bool SvApp::Window::EvaluateInput(EKey p_key, EKeyState p_state, EInputModifier p_modif)
{
    return  static_cast<int>(p_state) == glfwGetKey(m_window, static_cast<int>(p_key)) &&
            EvaluteModif(p_modif);
}

bool SvApp::Window::EvaluateInput(EMouseButton p_button, EMouseButtonState p_state, EInputModifier p_modif)
{
    return  static_cast<int>(p_state) == glfwGetMouseButton(m_window, static_cast<int>(p_button)) &&
            EvaluteModif(p_modif);
}

bool SvApp::Window::EvaluteModif(EInputModifier p_modif)
{
    int disiredModif = static_cast<int>(p_modif);
    int currentModif = 0;

    if (disiredModif < 0)
        return true;

    for (size_t i = 0; i < NUM_INPUT_MODIFIERS; i++)
    {
        int modif = 1 << i;
        int state = glfwGetKey(m_window, static_cast<int>(InputManager::GetModifKey(static_cast<EInputModifier>(modif))));
        if (state == static_cast<int>(EKeyState::PRESSED))
            currentModif += modif;
    }

    return disiredModif == currentModif;
}

void SvApp::Window::ToggleFullScreenMode()
{
    const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);
    glfwSetWindowMonitor(
        m_window, 
        m_isInFullscreen? nullptr: m_monitor,
        0, 
        m_isInFullscreen? m_yWindowedPos: 0, 
        mode->width, 
        mode->height, 
        mode->refreshRate);

    m_isInFullscreen = !m_isInFullscreen;
}

void SvApp::Window::Update()
{
    glfwPollEvents();
}


void SvApp::Window::EndRender()
{
    glfwSwapBuffers(m_window);
}

bool SvApp::Window::ShouldClose()
{
    return glfwWindowShouldClose(m_window);
}

void SvApp::Window::SetWindowSizeLimits(int p_minWidth, int p_minHeight, int p_maxWidth, int p_maxHeight)
{
    glfwSetWindowSizeLimits(m_window, p_minWidth, p_minHeight, p_maxWidth, p_maxHeight);
}

void SvApp::Window::SetAspectRatio(int p_width, int p_height)
{
    glfwSetWindowAspectRatio(m_window, p_width, p_height);
}

void SvApp::Window::SetWindowIcons(std::vector<GLFWimage> p_images)
{
    glfwSetWindowIcon(m_window, static_cast<int>(p_images.size()), p_images.data());
}

void SvApp::Window::SetFocusWindow()
{
    glfwFocusWindow(m_window);
}

void SvApp::Window::WindowCloseRequest::BeforeInvoke()
{
    m_cancelRequest = false;
}

void SvApp::Window::WindowCloseRequest::AfterInvoke()
{
    if (!m_cancelRequest)
        SvCore::Events::EventManager::GetInstance().Invoke<WindowClosing>();
}

void SvApp::Window::WindowCloseRequest::InterceptCloseRequest()
{
    m_cancelRequest = true;
}
