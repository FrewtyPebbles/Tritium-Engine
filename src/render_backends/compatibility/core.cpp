#include "render_backends/compatibility/core.h"

void WindowManager::init_window(int width, int height)
{
	window = SDL_CreateWindow("Cool Game Engine",
							   SDL_WINDOWPOS_UNDEFINED,
							   SDL_WINDOWPOS_UNDEFINED,
							   width,
							   height,
							   0);
}

void WindowManager::close_window()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}
