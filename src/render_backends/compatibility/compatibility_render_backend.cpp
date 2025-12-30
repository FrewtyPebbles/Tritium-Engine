#include "Engine/render_backends/compatibility/compatibility_render_backend.h"

CompatibilityRenderBackend::CompatibilityRenderBackend(
	ApplicationConfig* application_config,
	Logger* logger,
	Uint32 sdl_only_window_flags
)
	: RenderBackend(
		application_config,
		logger
	)
{
	// sdl_window_flags must be set in every constructor to include `SDL_WINDOW_VULKAN`
	this->sdl_window_flags = SDL_WINDOW_OPENGL | sdl_only_window_flags;
}