# set the target name
get_filename_component(CURRENT_FOLDER_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET_NAME ${CURRENT_FOLDER_NAME})

# Download Lua
set(SURVIVANT_LUA_VERSION "5.4.6" CACHE INTERNAL "" FORCE)

FetchContent_Declare(
	${TARGET_NAME}
	GIT_REPOSITORY https://github.com/lua/lua.git
	GIT_TAG v${SURVIVANT_LUA_VERSION}
	GIT_SHALLOW ON
)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

FetchContent_MakeAvailable(${TARGET_NAME})

###############################
#                             #
# Sources                     #
#                             #
###############################

# Add source files
file(GLOB_RECURSE SOURCE_FILES
	${lua_SOURCE_DIR}/*.c
)

# Add header files
set(TARGET_INCLUDE_DIR ${lua_SOURCE_DIR})

file(GLOB_RECURSE HEADER_FILES
	${TARGET_INCLUDE_DIR}/*.h
)

source_group(TREE ${lua_SOURCE_DIR} FILES ${HEADER_FILES} ${SOURCE_FILES})


###############################
#                             #
# Library                     #
#                             #
###############################

add_library(${TARGET_NAME} STATIC ${HEADER_FILES} ${lua_SOURCE_DIR}/onelua.c)
target_include_directories(${TARGET_NAME} PRIVATE ${TARGET_INCLUDE_DIR})
target_compile_definitions(${TARGET_NAME} PRIVATE MAKE_LIB)

set(LUA_NAME ${TARGET_NAME} PARENT_SCOPE)
set(LUA_INCLUDE_DIR ${TARGET_INCLUDE_DIR} PARENT_SCOPE)
