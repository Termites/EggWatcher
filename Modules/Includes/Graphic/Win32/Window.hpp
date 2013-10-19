#ifndef _CORE_WIN32_WINDOW_HPP_
#define _CORE_WIN32_WINDOW_HPP_

#include <Windows.h>

#include <list>
#include <string>
#include <stdint.h>

namespace Graphic
{
	// User input : Key / Button / Joystick
	namespace Input
	{
		enum class KeyCode
		{
			Left,
			Right,
			Up,
			Down,


			A,
			B,
			C,
			D,
			E,
			F,
			G,
			H,
			I,
			J,
			K,
			L,
			M,
			N,
			O,
			P,
			Q,
			R,
			S,
			T,
			U,
			V,
			W,
			X,
			Y,
			Z,

			Numpad0,
			Numpad1,
			Numpad2,
			Numpad3,
			Numpad4,
			Numpad5,
			Numpad6,
			Numpad7,
			Numpad8,
			Numpad9,

			Mul,
			Add,
			Div,
			Sub,

			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,

			RShift,
			LShift,
			RCtrl,
			LCtrl,

			Enter,
			Back,
			Tab,
			Space,
			Escape,

			None
		};

		enum class MouseButton
		{
			LButton,
			RButton,
			MButton,

			WDButton,
			WUButton
		};
	}

	enum class WinEventType
	{
		Closed,
		Resized,

		KeyPressed,
		KeyReleased,

		MouseButtonPressed,
		MouseButtonReleased,

		MouseMoved,
		MouseWheel,

	};

	struct WindowEvent
	{
		WinEventType Type;

		Input::KeyCode KeyCode;
		Input::MouseButton MouseButton;

		int Width,Height;

		int MouseX,MouseY;
		int MouseWheelDelta;
	};

	class Window
	{
		private:
			HGLRC RenderingContext;
			HDC DeviceContext;
			HWND WindowHandle;
			HINSTANCE Instance;

			bool bFullScreen;
			bool bActive;

			std::list<WindowEvent> EventList;

			LARGE_INTEGER LastFrameTime;
			LARGE_INTEGER CurrentFrameTime;
			LARGE_INTEGER Frequency;

			double FrameTime;

			int Width,Height;

		public:
			Window();
			bool Create(const std::string& Title, int Width, int Height, int Bits, bool bFullScreen);

			bool GetEvent(WindowEvent& OutEvent);

			void PushEvent(const WindowEvent& Event);

			void SetActive(bool bActive)
			{
				this->bActive = bActive;
			}

			void Refresh();

			void Start();

			void Destroy();

			double GetFrameTime() const
			{
				return FrameTime;
			}

			int GetWidth() const {return Width;}
			int GetHeight() const {return Height;}

			void SetWidth(int Width)
			{
			    this->Width = Width;
			}

			void SetHeight(int Height)
			{
			    this->Height = Height;
			}

			~Window();
	};
};

#endif
