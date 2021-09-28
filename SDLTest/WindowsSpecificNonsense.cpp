#include <SDL.h>
#include "SDL_syswm.h"

#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

// we cannot just use WS_POPUP style
// WS_THICKFRAME: without this the window cannot be resized and so aero snap, de-maximizing and minimizing won't work
// WS_SYSMENU: enables the context menu with the move, close, maximize, minize... commands (shift + right-click on the task bar item)
// WS_CAPTION: enables aero minimize animation/transition
// WS_MAXIMIZEBOX, WS_MINIMIZEBOX: enable minimize/maximize
enum class Style : DWORD {
    windowed         = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
    aero_borderless  = WS_POPUP            | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
    basic_borderless = WS_POPUP            | WS_THICKFRAME              | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX
};

SDL_HitTestResult HitTestCallback(SDL_Window* win, const SDL_Point* area, void* data);

SDL_Window* CreateSdlWindow()
{
  //Create window
  SDL_SetHintWithPriority("SDL_BORDERLESS_RESIZABLE_STYLE", "1", SDL_HINT_OVERRIDE);
  SDL_SetHintWithPriority("SDL_BORDERLESS_WINDOWED_STYLE", "1", SDL_HINT_OVERRIDE);
  auto window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE);
  if (window == nullptr)
    return nullptr;

  SDL_SetWindowHitTest(window, &HitTestCallback, nullptr);
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(window, &wmInfo);
  HWND handle = wmInfo.info.win.window;
  
  //::SetWindowLongPtrW(handle, GWL_STYLE, static_cast<LONG>(Style::aero_borderless));

  // when switching between borderless and windowed, restore appropriate shadow state
  static const MARGINS shadow_state[2]{ { 0,0,0,0 },{ 1,1,1,1 } };
  ::DwmExtendFrameIntoClientArea(handle, &shadow_state[1]);

  // redraw frame
  //::SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
  //::ShowWindow(handle, SW_SHOW);

  return window;
}
