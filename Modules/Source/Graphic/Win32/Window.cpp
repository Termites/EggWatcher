#include <Graphic/Win32/Window.hpp>

#include <WindowsX.h>


Graphic::Input::KeyCode GetKeyCode(WPARAM Code)
{
    // A -> Z Keys
    if (Code >= 0x41 && Code <= 0x5A)
        return (Graphic::Input::KeyCode) ( int(Graphic::Input::KeyCode::A) + (Code - 0x41));

    // F1 -> F10 Keys
    if (Code >= VK_F1 && Code <= VK_F12)
       return (Graphic::Input::KeyCode) ( int(Graphic::Input::KeyCode::F1) + (Code - VK_F1));

    // Numpad0 -> Numpad9 Keys
    if (Code >= VK_NUMPAD0 && Code <= VK_NUMPAD9)
        return (Graphic::Input::KeyCode) ( int(Graphic::Input::KeyCode::Numpad0) + (Code - VK_NUMPAD0));

    // Arrows
    if (Code == VK_LEFT)
        return Graphic::Input::KeyCode::Left;
    else if (Code == VK_RIGHT)
        return Graphic::Input::KeyCode::Right;
    else if (Code == VK_UP)
        return Graphic::Input::KeyCode::Up;
    else if (Code == VK_DOWN)
        return Graphic::Input::KeyCode::Down;

    // Operators
    if (Code == VK_ADD)
        return Graphic::Input::KeyCode::Add;
    else if (Code == VK_SUBTRACT)
        return Graphic::Input::KeyCode::Sub;
    else if (Code == VK_MULTIPLY)
        return Graphic::Input::KeyCode::Mul;
    else if (Code == VK_DIVIDE)
        return Graphic::Input::KeyCode::Div;

    // Shift / Control
    if (Code == VK_LSHIFT)
        return Graphic::Input::KeyCode::LShift;
    else if (Code == VK_RSHIFT)
        return Graphic::Input::KeyCode::RShift;
    else if (Code == VK_LCONTROL)
        return Graphic::Input::KeyCode::LCtrl;
    else if (Code == VK_RCONTROL)
        return Graphic::Input::KeyCode::RCtrl;

    if (Code == VK_RETURN)
        return Graphic::Input::KeyCode::Enter;
    else if (Code == VK_BACK)
        return Graphic::Input::KeyCode::Back;
    else if (Code == VK_TAB)
        return Graphic::Input::KeyCode::Tab;
    else if (Code == VK_SPACE)
        return Graphic::Input::KeyCode::Space;
    else if (Code == VK_ESCAPE)
        return Graphic::Input::KeyCode::Escape;

    return Graphic::Input::KeyCode::None;
}

LRESULT CALLBACK WndProc(HWND WindowHandle,UINT uMSG,WPARAM wParam,LPARAM lParam)
{

    Graphic::Window* myWindow =(Graphic::Window*)GetWindowLong(WindowHandle,GWL_USERDATA);


    switch (uMSG)
    {
        case WM_ACTIVATE:
        {
            myWindow->SetActive(!HIWORD(wParam));
            return 0;
        }
        break;
        case WM_CLOSE:
        {
            Graphic::WindowEvent newEvent;
            newEvent.Type = Graphic::WinEventType::Closed;
            myWindow->PushEvent(newEvent);
            return 0;
        }
        break;

        case WM_KEYDOWN:
        {
            Graphic::WindowEvent newEvent;
            newEvent.Type = Graphic::WinEventType::KeyPressed;
            newEvent.KeyCode =  ( GetKeyCode(wParam) );
            myWindow->PushEvent(newEvent);
            return 0;
        }
        break;

        case WM_KEYUP:
        {
            Graphic::WindowEvent newEvent;
            newEvent.Type = Graphic::WinEventType::KeyReleased;
            newEvent.KeyCode =  ( GetKeyCode(wParam) );
            myWindow->PushEvent(newEvent);
            return 0;
        }
        break;

        case WM_MOUSEMOVE:
        {
            Graphic::WindowEvent newEvent;
            newEvent.Type = Graphic::WinEventType::MouseMoved;
            newEvent.MouseX = GET_X_LPARAM(lParam);
            newEvent.MouseY = GET_Y_LPARAM(lParam);
            myWindow->PushEvent(newEvent);

            return 0;
        }
        break;

        case WM_MOUSEWHEEL:
        {
            Graphic::WindowEvent newEvent;
            newEvent.Type = Graphic::WinEventType::MouseWheel;
            newEvent.MouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);//HIWORD(wParam) / float(WHEEL_DELTA);
            myWindow->PushEvent(newEvent);
            return 0;
        }
        break;

        case WM_LBUTTONDOWN:
        {
          Graphic::WindowEvent newEvent;
          newEvent.Type = Graphic::WinEventType::MouseButtonPressed;
          newEvent.MouseButton = Graphic::Input::MouseButton::LButton;
          myWindow->PushEvent(newEvent);
          return 0;
        }
        break;

        case WM_LBUTTONUP:
        {
          Graphic::WindowEvent newEvent;
          newEvent.Type = Graphic::WinEventType::MouseButtonReleased;
          newEvent.MouseButton = Graphic::Input::MouseButton::LButton;
          myWindow->PushEvent(newEvent);
          return 0;
        }
        break;

        case WM_RBUTTONDOWN:
        {
          Graphic::WindowEvent newEvent;
          newEvent.Type = Graphic::WinEventType::MouseButtonPressed;
          newEvent.MouseButton = Graphic::Input::MouseButton::RButton;
          myWindow->PushEvent(newEvent);
          return 0;
        }
        break;

        case WM_RBUTTONUP:
        {
          Graphic::WindowEvent newEvent;
          newEvent.Type = Graphic::WinEventType::MouseButtonReleased;
          newEvent.MouseButton = Graphic::Input::MouseButton::RButton;
          myWindow->PushEvent(newEvent);
          return 0;
        }
        break;

         case WM_MBUTTONDOWN:
        {
          Graphic::WindowEvent newEvent;
          newEvent.Type = Graphic::WinEventType::MouseButtonPressed;
          newEvent.MouseButton = Graphic::Input::MouseButton::MButton;
          myWindow->PushEvent(newEvent);
          return 0;
        }
        break;

        case WM_MBUTTONUP:
        {
          Graphic::WindowEvent newEvent;
          newEvent.Type = Graphic::WinEventType::MouseButtonReleased;
          newEvent.MouseButton = Graphic::Input::MouseButton::MButton;
          myWindow->PushEvent(newEvent);
          return 0;
        }
        break;

        case WM_SIZE:
        {
            Graphic::WindowEvent newEvent;
            newEvent.Type =Graphic::WinEventType::Resized;
            newEvent.Width = LOWORD(lParam);
            newEvent.Height = HIWORD(lParam);

            if (newEvent.Height <1)
                newEvent.Height = 1;

            if (newEvent.Width <1)
                newEvent.Width = 1;


            myWindow->SetWidth(newEvent.Width);
            myWindow->SetHeight(newEvent.Height);
            myWindow->PushEvent(newEvent);
            return 0;

        }
        break;
    }
    return DefWindowProc(WindowHandle,uMSG,wParam,lParam);
}

Graphic::Window::Window()
{
    bActive=true;
    RenderingContext=NULL;
    DeviceContext=NULL;
    WindowHandle=NULL;

    QueryPerformanceFrequency(&Frequency);

    bFullScreen = false;
    Instance = 0;
    Width = Height = 0;
    FrameTime = 1.f;
}

void Graphic::Window::PushEvent(const Graphic::WindowEvent& Event)
{
    if (!EventList.empty())
    {
        Graphic::WindowEvent & LastEvent = EventList.back();
        if (LastEvent.Type == WinEventType::Resized && Event.Type == WinEventType::Resized)
        {
            LastEvent.Width = Event.Width;
            LastEvent.Height = Event.Height;
            return;
        }
    }

    EventList.push_back(Event);
}

bool Graphic::Window::GetEvent(Graphic::WindowEvent& Event)
{
    MSG msg;

    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            Destroy();
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Avoid DeltaFrameTime error :
        //QueryPerformanceCounter(&CurrentFrameTime);
        //LastFrameTime = CurrentFrameTime;
    }

    if (EventList.empty() || !bActive)
        return false;

    Event = EventList.back();
    EventList.pop_back();


    return true;
}

bool Graphic::Window::Create(const std::string& Title,int32_t Width,int32_t Height, int Bits, bool bFullScreen)
{
    bool bExists=false;
    // In case the window already exists:
    if (Instance != NULL)
    {
        bExists=true;
        if (this->bFullScreen)
        {
            // Go-back to desktop mode :
            ChangeDisplaySettings(NULL,0);
        }

    }

    uint32_t PixelFormat;
    WNDCLASS WC;
    DWORD dwStyle=0;
    DWORD dwStyleExt=0;
    RECT WindowRect;

    this->Width = Width;
    this->Height = Height;

    // Setup window rectangle :
    WindowRect.left=(long)0;
    WindowRect.right=(long)Width;
    WindowRect.top=(long)0;
    WindowRect.bottom=(long)Height;

    this->bFullScreen = bFullScreen;


    if (Instance == NULL)
    {
        // Get an instance :
        Instance = GetModuleHandle(NULL);
        WC.style = CS_HREDRAW|CS_VREDRAW |CS_OWNDC; // Redraw on size, own DC.
        WC.lpfnWndProc =WndProc;
        WC.cbClsExtra = 0;
        WC.cbWndExtra = sizeof(void*);// Allocate a few bytes in order to store WindowController's instance address
        WC.hInstance = Instance;
        WC.hIcon = LoadIcon(NULL,IDI_WINLOGO); // default logo
        WC.hCursor = LoadCursor(NULL,IDC_ARROW); // default cursor
        WC.hbrBackground = NULL; // No background required
        WC.lpszMenuName = NULL; // no menu
        WC.lpszClassName="RAYENG_WIN";

        if (!RegisterClass(&WC))
        {
            // error :
            return false;
        }
    }

    if (this->bFullScreen)
    {
        DEVMODE DeviceModeSettings;
        memset(&DeviceModeSettings,0, sizeof(DeviceModeSettings));
        DeviceModeSettings.dmSize = sizeof(DeviceModeSettings);
        DeviceModeSettings.dmPelsWidth = Width;
        DeviceModeSettings.dmPelsHeight = Height;
        DeviceModeSettings.dmBitsPerPel  = Bits;
        DeviceModeSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        if (ChangeDisplaySettings(&DeviceModeSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            //DebugErrorMessage("Your graphic card cannot support full-screen ("+ValueToString(Width)+','+ValueToString(Height)+"), switching to windowed.",Error_Warning);
            this->bFullScreen = false;
        }
    }

    if (this->bFullScreen)
    {
        dwStyleExt = WS_EX_APPWINDOW;
        dwStyle=WS_POPUP;
    }
    else
    {
        dwStyleExt = WS_EX_APPWINDOW|WS_EX_WINDOWEDGE;
        dwStyle = WS_OVERLAPPEDWINDOW;
    }

    // Setup window using flags & size:
    AdjustWindowRectEx(&WindowRect,dwStyle,FALSE,dwStyleExt);

    // Create the window :
    if (!bExists)
        WindowHandle = CreateWindowEx(dwStyleExt,"RAYENG_WIN",Title.c_str(),dwStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,0,0,WindowRect.right - WindowRect.left,WindowRect.bottom - WindowRect.top,NULL,NULL,Instance,NULL);
    if (!WindowHandle)
    {
        Destroy();
        return false;
    }

    // Generate new context if it does not exists :
    if (DeviceContext == NULL)
    {
        DeviceContext = GetDC(WindowHandle);

        if (!DeviceContext)
        {
            // error :
            Destroy();
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd;
        ZeroMemory( &pfd, sizeof( pfd ) );
        pfd.nSize = sizeof( pfd );
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                      PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 16;
        pfd.iLayerType = PFD_MAIN_PLANE;

        PixelFormat = ChoosePixelFormat(DeviceContext,&pfd);
        if (!PixelFormat)
        {
            // error :

            Destroy();
            return false;
        }

        if (!SetPixelFormat(DeviceContext,PixelFormat,&pfd))
        {
            // error :

            Destroy();
            return false;
        }

        RenderingContext = wglCreateContext(DeviceContext);
        if (!RenderingContext)
        {
            Destroy();
            return false;
        }

        if (!wglMakeCurrent(DeviceContext,RenderingContext))
        {
            Destroy();
            return false;
        }
    }

    if (!bExists)
    {
        SetWindowLong(WindowHandle,  GWL_USERDATA,(LONG)this);
        ShowWindow(WindowHandle,SW_SHOW);
        SetForegroundWindow(WindowHandle);
        SetFocus(WindowHandle);
    }
    else
    {
        // Update style :
        SetWindowLong(WindowHandle,GWL_STYLE, dwStyle);
        SetWindowLong(WindowHandle,GWL_EXSTYLE,dwStyleExt);
        ShowWindow(WindowHandle,SW_SHOW);
        SetForegroundWindow(WindowHandle);
        SetFocus(WindowHandle);
        SetWindowPos(WindowHandle,0,0,0,Width,Height,SWP_NOZORDER|SWP_NOACTIVATE);
    }


    return true;
}

void Graphic::Window::Start()
{
    QueryPerformanceCounter(&CurrentFrameTime);
    QueryPerformanceCounter(&LastFrameTime);
    FrameTime = 1.0f;
}

void Graphic::Window::Destroy()
{
    if (bFullScreen)
    {
        // Go-back to desktop mode :
        ChangeDisplaySettings(NULL,0);
        ShowCursor(TRUE);
        bFullScreen = false;
    }

    if (RenderingContext)
    {
        // Try to release Device & Rendering contextes :

        if (!wglMakeCurrent(NULL,NULL))
        {
            // Error :
        }

        // Try to delete the rendering context :
        if (!wglDeleteContext(RenderingContext))
        {

        }
        RenderingContext=NULL;
    }

    if (DeviceContext && !ReleaseDC(WindowHandle,DeviceContext))
    {
        // error :
    }
    RenderingContext=NULL;

    if (WindowHandle && !DestroyWindow(WindowHandle))
    {
        // error :
    }
    WindowHandle=NULL;

    if (!UnregisterClass("VG_WIN",Instance))
    {
        // error :
    }
    Instance=NULL;
}

void Graphic::Window::Refresh()
{
    QueryPerformanceCounter(&CurrentFrameTime);
    SwapBuffers(DeviceContext);

    FrameTime = (CurrentFrameTime.QuadPart - LastFrameTime.QuadPart) * 1000.0 / double(Frequency.QuadPart);
    LastFrameTime = CurrentFrameTime;

}

Graphic::Window::~Window()
{
    Destroy();
}
