# Download IMGUIZMO
FetchContent_Declare(
	imguizmo
	GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo
	GIT_TAG ba662b119d64f9ab700bb2cd7b2781f9044f5565
	GIT_SHALLOW ON
)

set(BUILD_SHARED_LIBS OFF)

FetchContent_MakeAvailable(imguizmo)

# set the target name
get_filename_component(CURRENT_FOLDER_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET_NAME ${CURRENT_FOLDER_NAME})


###############################
#                             #
# Sources                     #
#                             #
###############################

# Add source files
file(GLOB SOURCE_FILES ${imguizmo_SOURCE_DIR}/*.cpp)

# Add header files
set(TARGET_INCLUDE_DIR ${imguizmo_SOURCE_DIR})
file(GLOB HEADER_FILES ${imguizmo_SOURCE_DIR}/*.h)

###############################
#                             #
# Executable                  #
#                             #
###############################

add_library(${TARGET_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(${TARGET_NAME} PRIVATE ${TARGET_INCLUDE_DIR} ${IMGUI_INCLUDE_DIR})

source_group(TREE ${imguizmo_SOURCE_DIR} FILES ${HEADER_FILES} ${SOURCE_FILES})

set(IMGUIZMO_NAME ${TARGET_NAME} PARENT_SCOPE)
set(IMGUIZMO_INCLUDE_DIR ${TARGET_INCLUDE_DIR} PARENT_SCOPE)
