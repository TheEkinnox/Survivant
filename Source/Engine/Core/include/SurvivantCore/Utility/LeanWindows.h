#pragma once

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	// Disables unnecessary windows features
#endif

#ifndef NOMINMAX
#define NOMINMAX			// Avoids conflicts with LibMath's min and max
#endif

#undef APIENTRY				// Avoids conflicts with some other libs (e.g: GLFW)
#include <windows.h>

// Avoids conflict with input modifiers
#undef MOD_SHIFT
#undef MOD_CONTROL
#undef MOD_ALT

#endif // _WIN32
