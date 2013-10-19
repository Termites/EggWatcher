#ifndef _MODULE_GRAPHIC_WINDOW_HPP_
#define _MODULE_GRAPHIC_WINDOW_HPP_

#if defined(SYSTEM_WIN32)
	#include <Graphic/Win32/Window.hpp>
#elif defined(SYSTEM_UNIX)
	#include <Graphic/Unix/Window.hpp>
#else
	#error System not supported, aborting compilation.

#endif

#endif