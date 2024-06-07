set(BASE_FOLDER ${CMAKE_FOLDER})
string(TOUPPER ${PROJECT_NAME} PROJECT_PREFIX)

if (MSVC)
  add_compile_options(/Zc:preprocessor)
endif()

option(${PROJECT_PREFIX}_BUILD_TESTS "Enable to compile engine tests" ON)
option(${PROJECT_PREFIX}_BUILD_EDITOR "Enable to compile engine editor" ON)
cmake_dependent_option(${PROJECT_PREFIX}_BUILD_RUNTIME "Enable to compile engine base runtime application" ON "NOT ${PROJECT_PREFIX}_BUILD_EDITOR" ON)

set(CMAKE_FOLDER ${BASE_FOLDER}/Engine)
add_subdirectory(Engine)

set(CMAKE_FOLDER ${BASE_FOLDER})

if (${PROJECT_PREFIX}_BUILD_RUNTIME)
	add_subdirectory(Runtime)
	set(STARTUP_PROJECT ${RUNTIME_NAME})
endif()

if (${PROJECT_PREFIX}_BUILD_EDITOR)
	add_subdirectory(Editor)
	set(STARTUP_PROJECT ${EDITOR_NAME})
endif()

if (${PROJECT_PREFIX}_BUILD_TESTS)
	add_subdirectory(Test)
	set(STARTUP_PROJECT ${TEST_NAME})
endif()

if (DEFINED STARTUP_PROJECT)
	set (STARTUP_PROJECT ${STARTUP_PROJECT} PARENT_SCOPE)
endif()