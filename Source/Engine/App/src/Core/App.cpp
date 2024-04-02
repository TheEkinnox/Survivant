//App.cpp

#include "SurvivantApp/Core/App.h"

#include <GLFW/glfw3.h>


namespace SvApp::Core
{
	App::App()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	App::~App()
	{
		glfwTerminate();
	}
}

