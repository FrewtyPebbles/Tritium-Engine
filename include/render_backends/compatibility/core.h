#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>

//i think we should also run like a vulkan instance in here
//whenever you make a window...

namespace GameEngine::EngineUtils
{
	class WindowManager
	{
		public:
			void init_window(int width, int height);
			void close_window();
		private:
			SDL_Window *window = null;
	}
}
