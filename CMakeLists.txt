cmake_minimum_required(VERSION 3.30)

include(FetchContent)

# Arm64?
if((CMAKE_GENERATOR_PLATFORM MATCHES "ARM64") OR (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64") OR (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64"))
    set(IS_ARM64 TRUE)
else()
    set(IS_ARM64 FALSE)
endif()

# Self-build?
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}) # if not submodule
    get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

    if(IS_MULTI_CONFIG)
        set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "")
    endif()

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/_Bin" CACHE STRING "")
endif()

option(NRIF_USE_WAYLAND "Use Wayland instead of X11 on Linux" OFF)

# Create project
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/Include/NRIFramework.h" ver_h)
string(REGEX MATCH "NRI_FRAMEWORK_VERSION_MAJOR ([0-9]*)" _ ${ver_h})
set(VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "NRI_FRAMEWORK_VERSION_MINOR ([0-9]*)" _ ${ver_h})
set(VERSION_MINOR ${CMAKE_MATCH_1})

message("NRI framework v${VERSION_MAJOR}.${VERSION_MINOR}")
project(NRIFramework LANGUAGES C CXX)

if(APPLE)
    enable_language(OBJC)
endif()

# Globals?
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compile options
if(NOT IS_ARM64)
    set(SIMD -mssse3)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(COMPILE_OPTIONS ${SIMD} -Wextra)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(COMPILE_OPTIONS ${SIMD} -Wextra)
elseif(MSVC)
    set(COMPILE_OPTIONS /W4 /WX
        /wd4324 # padding was added at the end of a structure because you specified an alignment specifier
    )
else()
    message(WARNING "Unknown compiler!")
endif()

# Compile definitions
set(COMPILE_DEFINITIONS IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
if(WIN32)
    set(COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS)
elseif(NRIF_USE_WAYLAND)
    set(COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} NRIF_USE_WAYLAND=1)
endif()

# External/Detex
file(GLOB DETEX_SOURCE "External/Detex/*.c" "External/Detex/*.h")
source_group("" FILES ${DETEX_SOURCE})
add_library(detex STATIC ${DETEX_SOURCE})
target_compile_definitions(detex PRIVATE ${COMPILE_DEFINITIONS})
set_target_properties(detex PROPERTIES FOLDER "${PROJECT_NAME}")

# External/NRI
option(NRI_ENABLE_WAYLAND_SUPPORT "" ${NRIF_USE_WAYLAND})
option(NRI_ENABLE_IMGUI_EXTENSION "" ON)

add_subdirectory("External/NRI")

# External/MathLib
add_subdirectory("External/MathLib")

# External/MetalUtility
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(QUARTZ_CORE QuartzCore REQUIRED)
    add_library(MetalUtility SHARED "External/MetalUtility/MetalUtility.m")
    target_link_libraries(MetalUtility
        ${QUARTZ_CORE}
        ${COCOA_LIBRARY}
        glfw
    )
endif()

# Download dependencies
set(DEPS)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG master
    GIT_SHALLOW 1
)
list(APPEND DEPS imgui)

# GLFW
option(GLFW_BUILD_WAYLAND "" ${NRIF_USE_WAYLAND})
option(GLFW_BUILD_EXAMPLES "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
    GIT_SHALLOW 1
)
list(APPEND DEPS glfw)

# cgltf
FetchContent_Declare(
    cgltf
    GIT_REPOSITORY https://github.com/jkuhlmann/cgltf.git
    GIT_TAG master
    GIT_SHALLOW 1
)
list(APPEND DEPS cgltf)

if(NOT TARGET ShaderMake) # NRI could add it already
    # ShaderMake
    option(SHADERMAKE_TOOL "" ON)

    FetchContent_Declare(
        shadermake
        GIT_REPOSITORY https://github.com/NVIDIA-RTX/ShaderMake.git
        GIT_TAG 757594f0539314467f74950c8d898e815ecbd383
        GIT_SHALLOW 1
    )
    list(APPEND DEPS shadermake)
endif()

if(DEPS)
    message("NRI framework: downloading dependencies:")
    message(STATUS "${DEPS} ...")

    FetchContent_MakeAvailable(${DEPS})
endif()

# External/Imgui
file(GLOB IMGUI_SOURCE "${imgui_SOURCE_DIR}/*.cpp" "${imgui_SOURCE_DIR}/*.h")
source_group("" FILES ${IMGUI_SOURCE})
add_library(imgui STATIC ${IMGUI_SOURCE})
target_compile_definitions(imgui PRIVATE ${COMPILE_DEFINITIONS})
target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}")
set_target_properties(imgui PROPERTIES FOLDER "${PROJECT_NAME}")

# NRIFramework
file(GLOB FRAMEWORK_SOURCE "Source/*.cpp" "Source/*.h")
source_group("Source" FILES ${FRAMEWORK_SOURCE})
file(GLOB FRAMEWORK_HEADERS "Include/*.hpp" "Include/*.h" "${cgltf_SOURCE_DIR}/cgltf.h")
source_group("Include" FILES ${FRAMEWORK_HEADERS})
add_library(${PROJECT_NAME} STATIC ${FRAMEWORK_SOURCE} ${FRAMEWORK_HEADERS})
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        detex
        glfw
    PUBLIC
        NRI
        imgui
        MathLib
)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        "Source"
        "External"
        "${cgltf_SOURCE_DIR}"
        $<$<BOOL:${NRIF_USE_WAYLAND}>:/usr/include/wayland> # TODO: looks like a workaround...
    PUBLIC
        "Include"
        "${glfw_SOURCE_DIR}/include"
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_DL_LIBS}
        pthread
        MetalUtility
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_DL_LIBS}
        pthread
        X11
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE ${COMPILE_DEFINITIONS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${PROJECT_NAME}")

# AgilitySDK requires specific symbols to be exported from the executable, which are not needed elsewhere.
# Any other method doesn't work. "PUBLIC" to make settings visible in the parent project (EXE).
if(MSVC)
    include(CheckLinkerFlag)
    check_linker_flag(CXX /NOEXP HAS_NOEXP)

    set(LINK_OPTIONS /NOIMPLIB)
    if(HAS_NOEXP)
        set(LINK_OPTIONS ${LINK_OPTIONS} /NOEXP)
    endif()

    target_link_options(${PROJECT_NAME} PUBLIC ${LINK_OPTIONS})
endif()
