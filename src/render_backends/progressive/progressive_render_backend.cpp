#include "Engine/render_backends/progressive/progressive_render_backend.h"

// ==== Class Functions ====

ProgressiveRenderBackend::ProgressiveRenderBackend(Uint32 sdl_only_window_flags) {
	// sdl_window_flags must be set in every constructor to include `SDL_WINDOW_VULKAN`
	this->sdl_window_flags = SDL_WINDOW_VULKAN | sdl_only_window_flags;
}