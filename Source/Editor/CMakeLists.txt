# set target
get_filename_component(CURRENT_FOLDER_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET_NAME ${PROJECT_NAME}${CURRENT_FOLDER_NAME})

# Setup output directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/${TARGET_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/${TARGET_NAME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})


###############################
#                             #
# Sources                     #
#                             #
###############################

# Add source files
file(GLOB_RECURSE SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cc # C with classe
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/*.c++
)

# Add header files
set(TARGET_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.inl
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES} ${SOURCE_FILES})


###############################
#                             #
# Executable                  #
#                             #
###############################

add_executable(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${TARGET_NAME} PRIVATE ${TARGET_INCLUDE_DIR}
	${LIBMATH_INCLUDE_DIR} ${ENGINE_INCLUDE_DIRS}  ${IMGUI_INCLUDE_DIR} ${IMGUIZMO_INCLUDE_DIR}
)

target_link_libraries(${TARGET_NAME}
	PRIVATE
	${LIBMATH_NAME} ${IMGUI_NAME}
	${ENGINE_TARGETS} ${IMGUIZMO_NAME}
)

target_compile_definitions(${TARGET_NAME} PUBLIC SV_EDITOR SV_VERBOSE_LOG)

if(MSVC)
	target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
	target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

copy_resources(${TARGET_NAME})

if (${PROJECT_PREFIX}_BUILD_RUNTIME)
	set(${PROJECT_PREFIX}_RUNTIME_DIR "runtime" CACHE STRING "The editor's pre-compiled runtime binaries target path")
	option(${PROJECT_PREFIX}_BUILD_RUNTIME_ALL_CONFIGS "Enable to always compile all configurations of the base runtime application with the editor" OFF)

	if(${PROJECT_PREFIX}_BUILD_RUNTIME_ALL_CONFIGS)
		add_dependencies(${TARGET_NAME} ${RUNTIME_ALL_CONFIGS_NAME})
	else()
		add_dependencies(${TARGET_NAME} ${RUNTIME_NAME})
	endif()

	# Create runtimes symlink
	get_target_property(RUNTIME_BUILD_PATH ${RUNTIME_NAME} RUNTIME_OUTPUT_DIRECTORY)
	set(RUNTIME_EDITOR_PATH $<TARGET_FILE_DIR:${TARGET_NAME}>/${${PROJECT_PREFIX}_RUNTIME_DIR})

	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E create_symlink ${RUNTIME_BUILD_PATH} ${RUNTIME_EDITOR_PATH}
	)

	# Set runtime utility macros
	list(JOIN CMAKE_CONFIGURATION_TYPES "\",\"" CONFIG_TYPES)

	target_compile_definitions(${TARGET_NAME} PRIVATE
		SV_RUNTIME_DIR="${${PROJECT_PREFIX}_RUNTIME_DIR}"
		SV_RUNTIME_EXE="$<TARGET_FILE_NAME:${RUNTIME_NAME}>"
		SV_RUNTIME_CONFIGS="${CONFIG_TYPES}"
	)
endif()

set(EDITOR_NAME ${TARGET_NAME} PARENT_SCOPE)