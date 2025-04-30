// © 2021 NVIDIA Corporation

#pragma once

#define NRI_FRAMEWORK_VERSION_MAJOR 0
#define NRI_FRAMEWORK_VERSION_MINOR 20
#define NRI_FRAMEWORK_VERSION_DATE "28 April 2025"
#define NRI_FRAMEWORK 1

// Platform detection
#define NRIF_WINDOWS 0
#define NRIF_X11 1
#define NRIF_WAYLAND 2
#define NRIF_COCOA 3

#if defined(_WIN32)
#    define NRIF_PLATFORM NRIF_WINDOWS
#    define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#    define NRIF_PLATFORM NRIF_COCOA
#    define GLFW_EXPOSE_NATIVE_COCOA
#elif (defined(__linux__) && NRIF_USE_WAYLAND)
#    define NRIF_PLATFORM NRIF_WAYLAND
#    define GLFW_EXPOSE_NATIVE_WAYLAND
#elif (defined(__linux__))
#    define NRIF_PLATFORM NRIF_X11
#    define GLFW_EXPOSE_NATIVE_X11
#else
#    error "Unknown platform"
#endif

#include <array>
#include <cinttypes>
#include <cstdlib>
#include <string>
#include <vector>

// 3rd party
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "imgui.h"

// NRI: core & common extensions
#include "NRI.h"

#include "Extensions/NRIDeviceCreation.h"
#include "Extensions/NRIHelper.h"
#include "Extensions/NRIImgui.h"
#include "Extensions/NRILowLatency.h"
#include "Extensions/NRIMeshShader.h"
#include "Extensions/NRIRayTracing.h"
#include "Extensions/NRIResourceAllocator.h"
#include "Extensions/NRIStreamer.h"
#include "Extensions/NRISwapChain.h"
#include "Extensions/NRIUpscaler.h"

// 3rd party
#include "CmdLine.h" // https://github.com/tanakh/cmdline

#include "ml.h"
#include "ml.hlsli"

// NRI framework
#include "Camera.h"
#include "Controls.h"
#include "Helper.h"
#include "Timer.h"
#include "Utils.h"

// Settings
constexpr nri::VKBindingOffsets VK_BINDING_OFFSETS = {0, 128, 32, 64}; // see CMake
constexpr bool D3D11_COMMANDBUFFER_EMULATION = false;
constexpr uint32_t DEFAULT_MEMORY_ALIGNMENT = 16;

struct BackBuffer {
    nri::Descriptor* colorAttachment;
    nri::Texture* texture;
    nri::Format attachmentFormat;
};

class SampleBase {
public:
    // Pre initialize
    SampleBase();

    virtual void InitCmdLine([[maybe_unused]] cmdline::parser& cmdLine) {
    }

    virtual void ReadCmdLine([[maybe_unused]] cmdline::parser& cmdLine) {
    }

    // Initialize
    virtual bool Initialize(nri::GraphicsAPI graphicsAPI) = 0;
    bool InitUI(nri::Device& device);

    // Wait before input (wait for latency and/or queued frames)
    virtual void LatencySleep([[maybe_unused]] uint32_t frameIndex) {
    }

    // Prepare
    virtual void PrepareFrame([[maybe_unused]] uint32_t frameIndex) {
    }

    // UI
    void BeginUI();
    //      Imgui::
    void EndUI();

    // Render
    virtual void RenderFrame(uint32_t frameIndex) = 0;
    void RenderUI(nri::CommandBuffer& commandBuffer, nri::Streamer& streamer, nri::Format attachmentFormat, float sdrScale, bool isSrgb);

    // Destroy
    virtual ~SampleBase();
    void DestroyUI();

    // Misc
    virtual bool AppShouldClose() {
        return false;
    }

    inline bool IsKeyToggled(Key key) {
        bool state = m_KeyToggled[(uint32_t)key];
        m_KeyToggled[(uint32_t)key] = false;

        return state;
    }

    inline bool IsKeyPressed(Key key) const {
        return m_KeyState[(uint32_t)key];
    }

    inline bool IsButtonPressed(Button button) const {
        return m_ButtonState[(uint8_t)button];
    }

    inline const float2& GetMouseDelta() const {
        return m_MouseDelta;
    }

    inline float GetMouseWheel() const {
        return m_MouseWheel;
    }

    inline uint2 GetWindowResolution() const {
        return m_WindowResolution;
    }

    inline uint2 GetOutputResolution() const {
        return m_OutputResolution;
    }

    inline const nri::Window& GetWindow() const {
        return m_NRIWindow;
    }

    inline uint8_t GetQueuedFrameNum() const {
        return m_VsyncInterval ? 2 : 3;
    }

    inline uint8_t GetSwapChainFrameNum() const {
        return GetQueuedFrameNum() + 1;
    }

    void GetCameraDescFromInputDevices(CameraDesc& cameraDesc);

    static void EnableMemoryLeakDetection(uint32_t breakOnAllocationIndex);

protected:
    nri::AllocationCallbacks m_AllocationCallbacks = {};
    std::string m_SceneFile = "ShaderBalls/ShaderBalls.gltf";
    GLFWwindow* m_Window = nullptr;
    Camera m_Camera;
    Timer m_Timer;
    uint2 m_OutputResolution = {1920, 1080};
    uint2 m_WindowResolution = {};
    uint8_t m_VsyncInterval = 0;
    uint32_t m_DpiMode = 0;
    uint32_t m_RngState = 0;
    uint32_t m_AdapterIndex = 0;
    float m_MouseSensitivity = 1.0f;
    bool m_DebugAPI = false;
    bool m_DebugNRI = false;
    bool m_IsActive = true;

    // Private
private:
    void CursorMode(int32_t mode);

public:
    inline bool HasUserInterface() const {
        return m_TimePrev != 0.0;
    }

    void InitCmdLineDefault(cmdline::parser& cmdLine);
    void ReadCmdLineDefault(cmdline::parser& cmdLine);
    bool Create(int32_t argc, char** argv, const char* windowTitle);
    void RenderLoop();

    // Input (not public)
public:
    std::array<bool, (size_t)Key::NUM> m_KeyState = {};
    std::array<bool, (size_t)Key::NUM> m_KeyToggled = {};
    std::array<bool, (size_t)Button::NUM> m_ButtonState = {};
    std::array<bool, (size_t)Button::NUM> m_ButtonJustPressed = {};
    float2 m_MouseDelta = {};
    float2 m_MousePosPrev = {};
    float m_MouseWheel = 0.0f;

private:
    // UI
    nri::ImguiInterface m_iImgui = {};
    nri::Imgui* m_ImguiRenderer = nullptr;
    GLFWcursor* m_MouseCursors[ImGuiMouseCursor_COUNT] = {};
    double m_TimePrev = 0.0;

    // Rendering
    nri::Window m_NRIWindow = {};
    uint32_t m_FrameNum = uint32_t(-1);
};

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

#define NRI_ABORT_ON_FAILURE(result) \
    if ((result) != nri::Result::SUCCESS) { \
        exit(1); \
    }

#define NRI_ABORT_ON_FALSE(result) \
    if (!(result)) { \
        exit(1); \
    }

#define SAMPLE_MAIN(className, memoryAllocationIndexForBreak) \
    SampleBase* g_sample = nullptr; \
    void Destroy() { \
        if (g_sample) \
            delete g_sample; \
    } \
    int main(int argc, char** argv) { \
        SampleBase::EnableMemoryLeakDetection(memoryAllocationIndexForBreak); \
        g_sample = new className; \
        atexit(Destroy); \
        bool result = g_sample->Create(argc, argv, STRINGIFY(PROJECT_NAME)); \
        if (result) \
            g_sample->RenderLoop(); \
        return result ? 0 : 1; \
    }
