# Download PhysX
FetchContent_Declare(
	physx
	GIT_REPOSITORY https://github.com/NVIDIA-Omniverse/PhysX
	GIT_TAG 105.1-physx-5.3.1
	GIT_SHALLOW ON
)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(BUILD_SHARED_LIBS OFF)
set(PUBLIC_RELEASE OFF CACHE INTERNAL "")
set(TARGET_BUILD_PLATFORM "windows")

set(PX_OUTPUT_LIB_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} CACHE INTERNAL "")
set(PX_OUTPUT_BIN_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} CACHE INTERNAL "")
set(PX_GENERATE_SOURCE_DISTRO ON CACHE INTERNAL "")
set(PX_COPY_EXTERNAL_DLL OFF CACHE INTERNAL "")
set(NV_USE_DEBUG_WINCRT ON CACHE INTERNAL "") # When off PhysX defines both NDEBUG and _DEBUG in debug builds which it doesn't like

if (BUILD_SHARED_LIBS)
    set(PX_GENERATE_STATIC_LIBRARIES OFF CACHE INTERNAL "")
else()
    set(PX_GENERATE_STATIC_LIBRARIES ON CACHE INTERNAL "")
endif()

FetchContent_MakeAvailable(physx)

set(PHYSX_ROOT_DIR ${physx_SOURCE_DIR}/physx)
list(APPEND CMAKE_MODULE_PATH ${PHYSX_ROOT_DIR}/source/compiler/cmake/modules)

add_subdirectory(${PHYSX_ROOT_DIR}/source/compiler/cmake build)

if (MSVC)
    # Disable the following warnings for compatibility with C++20:
    # - C5054: binary operation between enums of different types
    # - C5055: operations between enum and floating-point types
    target_compile_options(PhysX PRIVATE -wd5054)
    target_compile_options(PhysXExtensions PRIVATE -wd5054)
    target_compile_options(PhysXCommon PRIVATE -wd5054 -wd5055)
    target_compile_options(SimulationController PRIVATE -wd5054)
endif()

set(PHYSX_LIBRARIES PhysX PhysXCommon PhysXFoundation PhysXExtensions PARENT_SCOPE)
set(PHYSX_INCLUDE_DIR ${PHYSX_ROOT_DIR}/include PARENT_SCOPE)