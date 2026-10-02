cmake_minimum_required(VERSION 3.30)

include(FetchContent)

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

# Arm64?
if((CMAKE_GENERATOR_PLATFORM MATCHES "ARM64") OR (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64"))
    set(IS_ARM64 TRUE)
else()
    set(IS_ARM64 FALSE)
endif()

if(APPLE)
    enable_language(OBJC)
endif()

# Enable grouping of source files into folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Compile definitions
set(COMPILE_DEFINITIONS IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
if(WIN32)
    list(APPEND COMPILE_DEFINITIONS WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS)
elseif(NRIF_USE_WAYLAND)
    list(APPEND COMPILE_DEFINITIONS NRIF_USE_WAYLAND=1)
endif()

# External/Detex
set(DETEX_SOURCE
    "External/Detex/bits.c"
    "External/Detex/bits.h"
    "External/Detex/bptc-tables.c"
    "External/Detex/bptc-tables.h"
    "External/Detex/clamp.c"
    "External/Detex/convert.c"
    "External/Detex/dds.c"
    "External/Detex/decompress-bc.c"
    "External/Detex/decompress-bptc.c"
    "External/Detex/decompress-bptc-float.c"
    "External/Detex/decompress-eac.c"
    "External/Detex/decompress-etc.c"
    "External/Detex/decompress-rgtc.c"
    "External/Detex/detex.h"
    "External/Detex/division-tables.c"
    "External/Detex/file-info.c"
    "External/Detex/file-info.h"
    "External/Detex/half-float.c"
    "External/Detex/half-float.h"
    "External/Detex/hdr.c"
    "External/Detex/hdr.h"
    "External/Detex/ktx.c"
    "External/Detex/misc.c"
    "External/Detex/misc.h"
    "External/Detex/stb.c"
    "External/Detex/stb_image.h"
    "External/Detex/stb_image_write.h"
    "External/Detex/texture.c"
)
source_group(""
    FILES
        ${DETEX_SOURCE}
)
add_library(detex STATIC)
target_sources(detex
    PRIVATE
        ${DETEX_SOURCE}
)
target_compile_definitions(detex
    PRIVATE
        ${COMPILE_DEFINITIONS}
)
set_target_properties(detex
    PROPERTIES
        FOLDER "${PROJECT_NAME}"
)

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
        PRIVATE
            ${QUARTZ_CORE}
            ${COCOA_LIBRARY}
            glfw
    )
endif()

# Download dependencies
set(DEPS)

# ImGui
fetchcontent_declare(
    imgui
    DOWNLOAD_EXTRACT_TIMESTAMP 1
    DOWNLOAD_NO_PROGRESS 1
    SOURCE_SUBDIR "ignore CMakeLists.txt"
    URL "https://github.com/ocornut/imgui/archive/refs/tags/v1.92.9b.zip"
)
list(APPEND DEPS imgui)

# GLFW
option(GLFW_BUILD_WAYLAND "" ${NRIF_USE_WAYLAND})
option(GLFW_BUILD_EXAMPLES "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)

fetchcontent_declare(
    glfw
    DOWNLOAD_EXTRACT_TIMESTAMP 1
    DOWNLOAD_NO_PROGRESS 1
    URL "https://github.com/glfw/glfw/archive/refs/tags/3.5.1.zip"
)
list(APPEND DEPS glfw)

# cgltf
fetchcontent_declare(
    cgltf
    DOWNLOAD_EXTRACT_TIMESTAMP 1
    DOWNLOAD_NO_PROGRESS 1
    SOURCE_SUBDIR "ignore CMakeLists.txt"
    URL "https://github.com/jkuhlmann/cgltf/archive/85cd62382dfea638278962690cf515023f33ed00.zip"
)
list(APPEND DEPS cgltf)

if(NOT TARGET ShaderMake) # NRI could add it already
    # ShaderMake
    option(SHADERMAKE_TOOL "" ON)

    fetchcontent_declare(
        shadermake
        DOWNLOAD_EXTRACT_TIMESTAMP 1
        DOWNLOAD_NO_PROGRESS 1
        URL "https://github.com/NVIDIA-RTX/ShaderMake/archive/9c978c37ccd1c00b8669d633735823fe25ee3647.zip"
    )
    list(APPEND DEPS shadermake)
endif()

if(DEPS)
    message("NRI framework: downloading dependencies:")
    message(STATUS "${DEPS} ...")

    fetchcontent_makeavailable(${DEPS})
endif()

# External/Imgui
file(GLOB IMGUI_SOURCE "${imgui_SOURCE_DIR}/*.cpp" "${imgui_SOURCE_DIR}/*.h")
source_group(""
    FILES
        ${IMGUI_SOURCE}
)
add_library(imgui STATIC)
target_sources(imgui
    PRIVATE
        ${IMGUI_SOURCE}
)
target_compile_definitions(imgui
    PRIVATE
        ${COMPILE_DEFINITIONS}
)
target_include_directories(imgui
    PUBLIC
        "${imgui_SOURCE_DIR}"
)
target_compile_features(imgui
    PRIVATE
        cxx_std_17
)
set_target_properties(imgui
    PROPERTIES
        FOLDER "${PROJECT_NAME}"
)

# NRIFramework
set(FRAMEWORK_SOURCE
    "Source/Camera.cpp"
    "Source/DebugAllocator.cpp"
    "Source/SampleBase.cpp"
    "Source/Timer.cpp"
    "Source/Utils.cpp"
)
source_group("Source"
    FILES
        ${FRAMEWORK_SOURCE}
)
set(FRAMEWORK_HEADERS
    "Include/Camera.h"
    "Include/CmdLine.h"
    "Include/Controls.h"
    "Include/Helper.h"
    "Include/NRIFramework.h"
    "Include/Timer.h"
    "Include/Utils.h"
    "${cgltf_SOURCE_DIR}/cgltf.h"
)
source_group("Include"
    FILES
        ${FRAMEWORK_HEADERS}
)
add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME}
    PRIVATE
        ${FRAMEWORK_SOURCE}
        ${FRAMEWORK_HEADERS}
)
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
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            ${CMAKE_DL_LIBS}
            pthread
            MetalUtility
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            ${CMAKE_DL_LIBS}
            pthread
            X11
    )
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        ${COMPILE_DEFINITIONS}
)
target_compile_options(${PROJECT_NAME}
    PRIVATE
        # Options for Clang/GCC compilers
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
            $<$<NOT:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>:
                -Wextra
                $<$<NOT:$<BOOL:${IS_ARM64}>>:-mssse3>
            >
        >
        # Options for MSVC frontend
        $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:
            /W4
            /WX
            /wd4324 # padding was added at the end of a structure because you specified an alignment specifier
        >
)
target_compile_features(${PROJECT_NAME}
    PUBLIC
        cxx_std_17
)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        FOLDER "${PROJECT_NAME}"
)

# AgilitySDK requires specific symbols to be exported from the executable, which are not needed elsewhere.
# Any other method doesn't work. "PUBLIC" to make settings visible in the parent project (EXE).
if(MSVC)
    include(CheckLinkerFlag)
    check_linker_flag(CXX /NOEXP HAS_NOEXP)

    set(LINK_OPTIONS /NOIMPLIB)
    if(HAS_NOEXP)
        set(LINK_OPTIONS ${LINK_OPTIONS} /NOEXP)
    endif()

    target_link_options(${PROJECT_NAME}
        PUBLIC
            ${LINK_OPTIONS}
    )
endif()
