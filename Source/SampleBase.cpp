// © 2021 NVIDIA Corporation

#include "NRIFramework.h"

#if (NRIF_PLATFORM == NRIF_WINDOWS)
#    include "NRIAgilitySDK.h"
#else
#    include <csignal>
#endif

#if (NRIF_PLATFORM == NRIF_COCOA)
#    include "MetalUtility/MetalUtility.h"
#endif

#include "GLFW/glfw3native.h"

template <typename T>
constexpr void MaybeUnused([[maybe_unused]] const T& arg) {
}

void CreateDebugAllocator(nri::AllocationCallbacks& allocationCallbacks);
void DestroyDebugAllocator(nri::AllocationCallbacks& allocationCallbacks);

//==================================================================================================================================================
// GLFW CALLBACKS
//==================================================================================================================================================

static ImGuiKey RemapKey(int32_t key) {
    switch (key) {
        case GLFW_KEY_SPACE:
            return ImGuiKey_Space;
        case GLFW_KEY_APOSTROPHE:
            return ImGuiKey_Apostrophe;
        case GLFW_KEY_COMMA:
            return ImGuiKey_Comma;
        case GLFW_KEY_MINUS:
            return ImGuiKey_Minus;
        case GLFW_KEY_PERIOD:
            return ImGuiKey_Period;
        case GLFW_KEY_SLASH:
            return ImGuiKey_Slash;
        case GLFW_KEY_0:
            return ImGuiKey_0;
        case GLFW_KEY_1:
            return ImGuiKey_1;
        case GLFW_KEY_2:
            return ImGuiKey_2;
        case GLFW_KEY_3:
            return ImGuiKey_3;
        case GLFW_KEY_4:
            return ImGuiKey_4;
        case GLFW_KEY_5:
            return ImGuiKey_5;
        case GLFW_KEY_6:
            return ImGuiKey_6;
        case GLFW_KEY_7:
            return ImGuiKey_7;
        case GLFW_KEY_8:
            return ImGuiKey_8;
        case GLFW_KEY_9:
            return ImGuiKey_9;
        case GLFW_KEY_SEMICOLON:
            return ImGuiKey_Semicolon;
        case GLFW_KEY_EQUAL:
            return ImGuiKey_Equal;
        case GLFW_KEY_A:
            return ImGuiKey_A;
        case GLFW_KEY_B:
            return ImGuiKey_B;
        case GLFW_KEY_C:
            return ImGuiKey_C;
        case GLFW_KEY_D:
            return ImGuiKey_D;
        case GLFW_KEY_E:
            return ImGuiKey_E;
        case GLFW_KEY_F:
            return ImGuiKey_F;
        case GLFW_KEY_G:
            return ImGuiKey_G;
        case GLFW_KEY_H:
            return ImGuiKey_H;
        case GLFW_KEY_I:
            return ImGuiKey_I;
        case GLFW_KEY_J:
            return ImGuiKey_J;
        case GLFW_KEY_K:
            return ImGuiKey_K;
        case GLFW_KEY_L:
            return ImGuiKey_L;
        case GLFW_KEY_M:
            return ImGuiKey_M;
        case GLFW_KEY_N:
            return ImGuiKey_N;
        case GLFW_KEY_O:
            return ImGuiKey_O;
        case GLFW_KEY_P:
            return ImGuiKey_P;
        case GLFW_KEY_Q:
            return ImGuiKey_Q;
        case GLFW_KEY_R:
            return ImGuiKey_R;
        case GLFW_KEY_S:
            return ImGuiKey_S;
        case GLFW_KEY_T:
            return ImGuiKey_T;
        case GLFW_KEY_U:
            return ImGuiKey_U;
        case GLFW_KEY_V:
            return ImGuiKey_V;
        case GLFW_KEY_W:
            return ImGuiKey_W;
        case GLFW_KEY_X:
            return ImGuiKey_X;
        case GLFW_KEY_Y:
            return ImGuiKey_Y;
        case GLFW_KEY_Z:
            return ImGuiKey_Z;
        case GLFW_KEY_LEFT_BRACKET:
            return ImGuiKey_LeftBracket;
        case GLFW_KEY_BACKSLASH:
            return ImGuiKey_Backslash;
        case GLFW_KEY_RIGHT_BRACKET:
            return ImGuiKey_RightBracket;
        case GLFW_KEY_GRAVE_ACCENT:
            return ImGuiKey_GraveAccent;
        case GLFW_KEY_ESCAPE:
            return ImGuiKey_Escape;
        case GLFW_KEY_ENTER:
            return ImGuiKey_Enter;
        case GLFW_KEY_TAB:
            return ImGuiKey_Tab;
        case GLFW_KEY_BACKSPACE:
            return ImGuiKey_Backspace;
        case GLFW_KEY_INSERT:
            return ImGuiKey_Insert;
        case GLFW_KEY_DELETE:
            return ImGuiKey_Delete;
        case GLFW_KEY_RIGHT:
            return ImGuiKey_RightArrow;
        case GLFW_KEY_LEFT:
            return ImGuiKey_LeftArrow;
        case GLFW_KEY_DOWN:
            return ImGuiKey_DownArrow;
        case GLFW_KEY_UP:
            return ImGuiKey_UpArrow;
        case GLFW_KEY_PAGE_UP:
            return ImGuiKey_PageUp;
        case GLFW_KEY_PAGE_DOWN:
            return ImGuiKey_PageDown;
        case GLFW_KEY_HOME:
            return ImGuiKey_Home;
        case GLFW_KEY_END:
            return ImGuiKey_End;
        case GLFW_KEY_CAPS_LOCK:
            return ImGuiKey_CapsLock;
        case GLFW_KEY_SCROLL_LOCK:
            return ImGuiKey_ScrollLock;
        case GLFW_KEY_NUM_LOCK:
            return ImGuiKey_NumLock;
        case GLFW_KEY_PRINT_SCREEN:
            return ImGuiKey_PrintScreen;
        case GLFW_KEY_PAUSE:
            return ImGuiKey_Pause;
        case GLFW_KEY_F1:
            return ImGuiKey_F1;
        case GLFW_KEY_F2:
            return ImGuiKey_F2;
        case GLFW_KEY_F3:
            return ImGuiKey_F3;
        case GLFW_KEY_F4:
            return ImGuiKey_F4;
        case GLFW_KEY_F5:
            return ImGuiKey_F5;
        case GLFW_KEY_F6:
            return ImGuiKey_F6;
        case GLFW_KEY_F7:
            return ImGuiKey_F7;
        case GLFW_KEY_F8:
            return ImGuiKey_F8;
        case GLFW_KEY_F9:
            return ImGuiKey_F9;
        case GLFW_KEY_F10:
            return ImGuiKey_F10;
        case GLFW_KEY_F11:
            return ImGuiKey_F11;
        case GLFW_KEY_F12:
            return ImGuiKey_F12;
        case GLFW_KEY_KP_0:
            return ImGuiKey_Keypad0;
        case GLFW_KEY_KP_1:
            return ImGuiKey_Keypad1;
        case GLFW_KEY_KP_2:
            return ImGuiKey_Keypad2;
        case GLFW_KEY_KP_3:
            return ImGuiKey_Keypad3;
        case GLFW_KEY_KP_4:
            return ImGuiKey_Keypad4;
        case GLFW_KEY_KP_5:
            return ImGuiKey_Keypad5;
        case GLFW_KEY_KP_6:
            return ImGuiKey_Keypad6;
        case GLFW_KEY_KP_7:
            return ImGuiKey_Keypad7;
        case GLFW_KEY_KP_8:
            return ImGuiKey_Keypad8;
        case GLFW_KEY_KP_9:
            return ImGuiKey_Keypad9;
        case GLFW_KEY_KP_DECIMAL:
            return ImGuiKey_KeypadDecimal;
        case GLFW_KEY_KP_DIVIDE:
            return ImGuiKey_KeypadDivide;
        case GLFW_KEY_KP_MULTIPLY:
            return ImGuiKey_KeypadMultiply;
        case GLFW_KEY_KP_SUBTRACT:
            return ImGuiKey_KeypadSubtract;
        case GLFW_KEY_KP_ADD:
            return ImGuiKey_KeypadAdd;
        case GLFW_KEY_KP_ENTER:
            return ImGuiKey_KeypadEnter;
        case GLFW_KEY_KP_EQUAL:
            return ImGuiKey_KeypadEqual;
        case GLFW_KEY_LEFT_SHIFT:
            return ImGuiKey_LeftShift;
        case GLFW_KEY_LEFT_CONTROL:
            return ImGuiKey_LeftCtrl;
        case GLFW_KEY_LEFT_ALT:
            return ImGuiKey_LeftAlt;
        case GLFW_KEY_LEFT_SUPER:
            return ImGuiKey_LeftSuper;
        case GLFW_KEY_RIGHT_SHIFT:
            return ImGuiKey_RightShift;
        case GLFW_KEY_RIGHT_CONTROL:
            return ImGuiKey_RightCtrl;
        case GLFW_KEY_RIGHT_ALT:
            return ImGuiKey_RightAlt;
        case GLFW_KEY_RIGHT_SUPER:
            return ImGuiKey_RightSuper;
        case GLFW_KEY_MENU:
            return ImGuiKey_Menu;
        default:
            return ImGuiKey_None;
    }
}

static void GLFW_ErrorCallback(int32_t error, const char* message) {
    printf("GLFW error[%d]: %s\n", error, message);
#if (NRIF_PLATFORM == NRIF_WINDOWS)
    DebugBreak();
#else
    raise(SIGTRAP);
#endif
}

static void GLFW_KeyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
    MaybeUnused(scancode);
    MaybeUnused(mods);

    SampleBase* p = (SampleBase*)glfwGetWindowUserPointer(window);

    if (key < 0)
        return;

    p->m_KeyState[key] = action != GLFW_RELEASE;
    if (action != GLFW_RELEASE)
        p->m_KeyToggled[key] = true;

    if (p->HasUserInterface()) {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiKey remappedKey = RemapKey(key);
        io.AddKeyEvent(remappedKey, action == GLFW_PRESS);
    }
}

static void GLFW_CharCallback(GLFWwindow* window, uint32_t codepoint) {
    SampleBase* p = (SampleBase*)glfwGetWindowUserPointer(window);

    if (p->HasUserInterface()) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(codepoint);
    }
}

static void GLFW_ButtonCallback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods) {
    MaybeUnused(mods);

    SampleBase* p = (SampleBase*)glfwGetWindowUserPointer(window);

    p->m_ButtonState[button] = action != GLFW_RELEASE;

    if (p->HasUserInterface()) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(button, action == GLFW_PRESS);
    }
}

static void GLFW_CursorPosCallback(GLFWwindow* window, double x, double y) {
    SampleBase* p = (SampleBase*)glfwGetWindowUserPointer(window);

    float2 cursorPos = float2(float(x), float(y));
    p->m_MouseDelta = cursorPos - p->m_MousePosPrev;
    p->m_MousePosPrev = cursorPos;

    if (p->HasUserInterface()) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent(cursorPos.x, cursorPos.y);
    }
}

static void GLFW_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    SampleBase* p = (SampleBase*)glfwGetWindowUserPointer(window);

    p->m_MouseWheel = (float)yoffset;

    if (p->HasUserInterface()) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent((float)xoffset, (float)yoffset);
    }
}

//==================================================================================================================================================
// SAMPLE BASE
//==================================================================================================================================================

SampleBase::SampleBase() {
#if _DEBUG
    CreateDebugAllocator(m_AllocationCallbacks);
#endif
}

SampleBase::~SampleBase() {
    glfwTerminate();

#if _DEBUG
    if (m_AllocationCallbacks.userArg != nullptr)
        DestroyDebugAllocator(m_AllocationCallbacks);
#endif
}

void SampleBase::GetCameraDescFromInputDevices(CameraDesc& cameraDesc) {
    cameraDesc.timeScale = 0.025f * m_Timer.GetSmoothedFrameTime();

    if (!IsButtonPressed(Button::Right)) {
        CursorMode(GLFW_CURSOR_NORMAL);
        return;
    }

    CursorMode(GLFW_CURSOR_DISABLED);

    if (GetMouseWheel() > 0.0f)
        m_Camera.state.motionScale *= 1.1f;
    else if (GetMouseWheel() < 0.0f)
        m_Camera.state.motionScale /= 1.1f;

    float motionScale = m_Camera.state.motionScale;

    float2 mouseDelta = GetMouseDelta();
    cameraDesc.dYaw = -mouseDelta.x * m_MouseSensitivity;
    cameraDesc.dPitch = -mouseDelta.y * m_MouseSensitivity;

    if (IsKeyPressed(Key::Right))
        cameraDesc.dYaw -= motionScale;
    if (IsKeyPressed(Key::Left))
        cameraDesc.dYaw += motionScale;

    if (IsKeyPressed(Key::Up))
        cameraDesc.dPitch += motionScale;
    if (IsKeyPressed(Key::Down))
        cameraDesc.dPitch -= motionScale;

    if (IsKeyPressed(Key::W))
        cameraDesc.dLocal.z += motionScale;
    if (IsKeyPressed(Key::S))
        cameraDesc.dLocal.z -= motionScale;
    if (IsKeyPressed(Key::D))
        cameraDesc.dLocal.x += motionScale;
    if (IsKeyPressed(Key::A))
        cameraDesc.dLocal.x -= motionScale;
    if (IsKeyPressed(Key::E))
        cameraDesc.dLocal.y += motionScale;
    if (IsKeyPressed(Key::Q))
        cameraDesc.dLocal.y -= motionScale;
}

struct ImDrawVertOpt {
    float pos[2];
    uint32_t uv;
    uint32_t col;
};

bool SampleBase::InitImgui(nri::Device& device) {
    // Setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    float contentScale = 1.0f;
    if (m_DpiMode != 0) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();

        float unused;
        glfwGetMonitorContentScale(monitor, &contentScale, &unused);
    }

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameBorderSize = 1;
    style.WindowBorderSize = 1;
    style.ScaleAllSizes(contentScale);

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
    io.IniFilename = nullptr;

    m_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    m_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    m_MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR);
    m_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
    m_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
    m_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);

#if (NRIF_PLATFORM == NRIF_WINDOWS)
    m_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
    m_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
#endif

    // Font
    ImFontConfig fontConfig = {};
    fontConfig.SizePixels = floor(13.0f * contentScale);
    io.Fonts->AddFontDefault(&fontConfig);

    // Renderer
    nri::Result result = nri::nriGetInterface(device, NRI_INTERFACE(nri::ImguiInterface), &m_iImgui);
    if (result != nri::Result::SUCCESS)
        return false;

    nri::ImguiDesc imguiDesc = {};

    result = m_iImgui.CreateImgui(device, imguiDesc, m_ImguiRenderer);
    if (result != nri::Result::SUCCESS)
        return false;

    return true;
}

void SampleBase::DestroyImgui() {
    if (!HasUserInterface())
        return;

    m_iImgui.DestroyImgui(m_ImguiRenderer);
    ImGui::DestroyContext();

    m_ImguiRenderer = nullptr;
}

void SampleBase::CmdCopyImguiData(nri::CommandBuffer& commandBuffer, nri::Streamer& streamer) {
    if (!HasUserInterface())
        return;

    const ImDrawData& drawData = *ImGui::GetDrawData();

    nri::CopyImguiDataDesc copyImguiDataDesc = {};
    copyImguiDataDesc.drawLists = drawData.CmdLists.Data;
    copyImguiDataDesc.drawListNum = drawData.CmdLists.Size;
    copyImguiDataDesc.textures = drawData.Textures->Data;
    copyImguiDataDesc.textureNum = drawData.Textures->Size;

    m_iImgui.CmdCopyImguiData(commandBuffer, streamer, *m_ImguiRenderer, copyImguiDataDesc);
}

void SampleBase::CmdDrawImgui(nri::CommandBuffer& commandBuffer, nri::Format attachmentFormat, float sdrScale, bool isSrgb) {
    if (!HasUserInterface())
        return;

    const ImDrawData& drawData = *ImGui::GetDrawData();

    nri::DrawImguiDesc drawImguiDesc = {};
    drawImguiDesc.drawLists = drawData.CmdLists.Data;
    drawImguiDesc.drawListNum = drawData.CmdLists.Size;
    drawImguiDesc.displaySize = {(nri::Dim_t)drawData.DisplaySize.x, (nri::Dim_t)drawData.DisplaySize.y};
    drawImguiDesc.hdrScale = sdrScale;
    drawImguiDesc.attachmentFormat = attachmentFormat;
    drawImguiDesc.linearColor = !isSrgb;

    m_iImgui.CmdDrawImgui(commandBuffer, *m_ImguiRenderer, drawImguiDesc);
}

bool SampleBase::Create(int32_t argc, char** argv, const char* windowTitle) {
    // Command line
    cmdline::parser cmdLine;

    InitCmdLineDefault(cmdLine);
    InitCmdLine(cmdLine);

    bool parseStatus = cmdLine.parse(argc, argv);

    if (cmdLine.exist("help")) {
        printf("\n%s", cmdLine.usage().c_str());
        return false;
    }

    if (!parseStatus) {
        printf("\n%s\n\n%s", cmdLine.error().c_str(), cmdLine.usage().c_str());
        return false;
    }

    ReadCmdLineDefault(cmdLine);
    ReadCmdLine(cmdLine);

    // Init GLFW
    glfwSetErrorCallback(GLFW_ErrorCallback);

    if (!glfwInit())
        return false;

    // Window size
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    float contentScale = 1.0f;
    if (m_DpiMode != 0) {
        float unused;
        glfwGetMonitorContentScale(monitor, &contentScale, &unused);
        printf("DPI scale %.1f%% (%s)\n", contentScale * 100.0f, m_DpiMode == 2 ? "quality" : "performance");
    }

    m_WindowResolution.x = (uint32_t)floor(m_OutputResolution.x * contentScale);
    m_WindowResolution.y = (uint32_t)floor(m_OutputResolution.y * contentScale);

    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
    const uint32_t screenW = (uint32_t)vidmode->width;
    const uint32_t screenH = (uint32_t)vidmode->height;

    m_WindowResolution.x = min(m_WindowResolution.x, screenW);
    m_WindowResolution.y = min(m_WindowResolution.y, screenH);

    // Rendering output size
    m_OutputResolution.x = min(m_OutputResolution.x, m_WindowResolution.x);
    m_OutputResolution.y = min(m_OutputResolution.y, m_WindowResolution.y);

    if (m_DpiMode == 2)
        m_OutputResolution = m_WindowResolution;

    // Window creation
    bool decorated = m_WindowResolution.x != screenW && m_WindowResolution.y != screenH;

    printf("Creating %swindow (%u, %u)\n", decorated ? "" : "borderless ", m_WindowResolution.x, m_WindowResolution.y);

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, 0);
    glfwWindowHint(GLFW_DECORATED, decorated ? 1 : 0);
    glfwWindowHint(GLFW_RESIZABLE, 0);

    nri::GraphicsAPI graphicsAPI = nri::GraphicsAPI::VK;  // Default
    std::string selectedApi = cmdLine.get<std::string>("api");
    if (selectedApi == "D3D11") {
        graphicsAPI = nri::GraphicsAPI::D3D11;
    } else if (selectedApi == "D3D12") {
        graphicsAPI = nri::GraphicsAPI::D3D12;
    } else if (selectedApi == "VULKAN") {
        graphicsAPI = nri::GraphicsAPI::VK;
    }

    char windowName[256];
    snprintf(windowName, sizeof(windowName), "%s [%s]", windowTitle, nri::nriGetGraphicsAPIString(graphicsAPI));

    m_Window = glfwCreateWindow(m_WindowResolution.x, m_WindowResolution.y, windowName, NULL, NULL);
    if (!m_Window) {
        glfwTerminate();
        return false;
    }

#if (NRIF_PLATFORM != NRIF_WAYLAND)
    int32_t x = (screenW - m_WindowResolution.x) >> 1;
    int32_t y = (screenH - m_WindowResolution.y) >> 1;
    glfwSetWindowPos(m_Window, x, y); // GLFW error[65548]: Wayland: The platform does not support setting the window position
#endif

#if (NRIF_PLATFORM == NRIF_WINDOWS)
    m_NRIWindow.windows.hwnd = glfwGetWin32Window(m_Window);
#elif (NRIF_PLATFORM == NRIF_WAYLAND)
    m_NRIWindow.wayland.display = glfwGetWaylandDisplay();
    m_NRIWindow.wayland.surface = glfwGetWaylandWindow(m_Window);
#elif (NRIF_PLATFORM == NRIF_X11)
    m_NRIWindow.x11.dpy = glfwGetX11Display();
    m_NRIWindow.x11.window = glfwGetX11Window(m_Window);
#elif (NRIF_PLATFORM == NRIF_COCOA)
    m_NRIWindow.metal.caMetalLayer = GetMetalLayer(m_Window);
#endif

    // Main initialization
    printf("Loading...\n");

    bool result = Initialize(graphicsAPI, true);

    // Set callbacks and show window
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetKeyCallback(m_Window, GLFW_KeyCallback);
    glfwSetCharCallback(m_Window, GLFW_CharCallback);
    glfwSetMouseButtonCallback(m_Window, GLFW_ButtonCallback);
    glfwSetCursorPosCallback(m_Window, GLFW_CursorPosCallback);
    glfwSetScrollCallback(m_Window, GLFW_ScrollCallback);
    glfwShowWindow(m_Window);

    return result;
}

void SampleBase::RenderLoop() {
    double activeTime = 0.0;
    double imguiTimeStampPrev = 0;

    for (uint32_t i = 0; i < m_FrameNum; i++) {
        double timeCurr = glfwGetTime();

        LatencySleep(i);

        // Events
        glfwPollEvents();

        if (HasUserInterface()) {
            ImGuiIO& io = ImGui::GetIO();
            io.DisplaySize = ImVec2((float)m_WindowResolution.x, (float)m_WindowResolution.y);
            io.DeltaTime = (float)(glfwGetTime() - imguiTimeStampPrev);
            imguiTimeStampPrev = glfwGetTime();

            // Update key modifiers
            io.AddKeyEvent(ImGuiMod_Ctrl, IsKeyPressed(Key::LControl) || IsKeyPressed(Key::RControl));
            io.AddKeyEvent(ImGuiMod_Shift, IsKeyPressed(Key::LShift) || IsKeyPressed(Key::RShift));
            io.AddKeyEvent(ImGuiMod_Alt, IsKeyPressed(Key::LAlt) || IsKeyPressed(Key::RAlt));

            // Update mouse cursor
            if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0 && glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
                ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
                if (cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
                    // Hide OS mouse cursor if Imgui is drawing it or wants no cursor
                    CursorMode(GLFW_CURSOR_HIDDEN);
                } else {
                    // Show OS mouse cursor
                    glfwSetCursor(m_Window, m_MouseCursors[cursor] ? m_MouseCursors[cursor] : m_MouseCursors[ImGuiMouseCursor_Arrow]);
                    CursorMode(GLFW_CURSOR_NORMAL);
                }
            }
        }

        // Halting
        bool isActive = glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) != 0 && glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) == 0;
        isActive = m_AlwaysActive ? true : isActive;
        if (!isActive) {
            i--;
            continue;
        }

        // Closing?
        if (glfwWindowShouldClose(m_Window) || AppShouldClose())
            break;

        // Frame
        PrepareFrame(i);
        RenderFrame(i);

        // Finalize
        m_MouseWheel = 0.0f;
        m_MouseDelta = float2(0.0f);

        m_Timer.UpdateFrameTime();

        activeTime += glfwGetTime() - timeCurr;
        if (i > 2 && activeTime > m_TimeLimit)
            break;
    }

    printf(
        "FPS:\n"
        "  Last frame : %.2f fps (%.3f ms)\n"
        "  Average    : %.2f fps (%.3f ms)\n"
        "  Smoothed   : %.2f fps (%.3f ms)\n"
        "Shutting down...\n",
        1000.0f / m_Timer.GetFrameTime(), m_Timer.GetFrameTime(),
        1000.0f / m_Timer.GetSmoothedFrameTime(), m_Timer.GetSmoothedFrameTime(),
        1000.0f / m_Timer.GetVerySmoothedFrameTime(), m_Timer.GetVerySmoothedFrameTime());
}

void SampleBase::CursorMode(int32_t mode) {
    if (mode == GLFW_CURSOR_NORMAL) {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#if (NRIF_PLATFORM == NRIF_WINDOWS)
        // GLFW works with cursor visibility incorrectly
        for (uint32_t n = 0; ::ShowCursor(1) < 0 && n < 256; n++)
            ;
#endif
    } else {
        glfwSetInputMode(m_Window, GLFW_CURSOR, mode);
#if (NRIF_PLATFORM == NRIF_WINDOWS)
        // GLFW works with cursor visibility incorrectly
        for (uint32_t n = 0; ::ShowCursor(0) >= 0 && n < 256; n++)
            ;
#endif
    }
}

void SampleBase::InitCmdLineDefault(cmdline::parser& cmdLine) {
#if (NRIF_PLATFORM == NRIF_WINDOWS)
    std::string graphicsAPI = "D3D12";
#else
    std::string graphicsAPI = "VULKAN";
#endif

    cmdLine.add("help", '?', "print this message");
    cmdLine.add<std::string>("api", 'a', "graphics API: D3D11, D3D12 or VULKAN", false, graphicsAPI, cmdline::oneof<std::string>("D3D11", "D3D12", "VULKAN"));
    cmdLine.add<std::string>("scene", 's', "scene", false, m_SceneFile);
    cmdLine.add<uint32_t>("width", 'w', "output resolution width", false, m_OutputResolution.x);
    cmdLine.add<uint32_t>("height", 'h', "output resolution height", false, m_OutputResolution.y);
    cmdLine.add<uint32_t>("frameNum", 'f', "close after N frames", false, m_FrameNum);
    cmdLine.add<double>("timeLimit", 't', "close after N seconds", false, m_TimeLimit);
    cmdLine.add<uint32_t>("dpiMode", 0, "DPI mode", false, m_DpiMode);
    cmdLine.add<uint32_t>("adapter", 0, "Adapter index (0 - best)", false, m_AdapterIndex);
    cmdLine.add("vsync", 'v', "vertical sync");
    cmdLine.add("debugAPI", 0, "enable graphics API validation layer");
    cmdLine.add("debugNRI", 0, "enable NRI validation layer");
    cmdLine.add("alwaysActive", 0, "continue to render if not in focus");
}

void SampleBase::ReadCmdLineDefault(cmdline::parser& cmdLine) {
    m_SceneFile = cmdLine.get<std::string>("scene");
    m_OutputResolution.x = cmdLine.get<uint32_t>("width");
    m_OutputResolution.y = cmdLine.get<uint32_t>("height");
    m_FrameNum = cmdLine.get<uint32_t>("frameNum");
    m_TimeLimit = cmdLine.get<double>("timeLimit");
    m_DpiMode = cmdLine.get<uint32_t>("dpiMode");
    m_AdapterIndex = cmdLine.get<uint32_t>("adapter");
    m_Vsync = (uint8_t)cmdLine.exist("vsync");
    m_DebugAPI = cmdLine.exist("debugAPI");
    m_DebugNRI = cmdLine.exist("debugNRI");
    m_AlwaysActive = cmdLine.exist("alwaysActive");
}

void SampleBase::EnableMemoryLeakDetection([[maybe_unused]] uint32_t breakOnAllocationIndex) {
#if (defined(_DEBUG) && NRIF_PLATFORM == NRIF_WINDOWS)
    int32_t flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(flag);

    // https://msdn.microsoft.com/en-us/library/x98tx3cf.aspx
    if (breakOnAllocationIndex)
        _crtBreakAlloc = breakOnAllocationIndex;
#endif
}
