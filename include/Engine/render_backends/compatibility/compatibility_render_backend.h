#pragma once

#include "Engine/render_backends/render_backend.h"

class CompatibilityRenderBackend : public RenderBackend {

public:

	/////////////////////
	///// FUNCTIONS /////
	/////////////////////

	// ==== Class Functions ====
	// These include things like constructors, destructors and operators.
	// ---

	CompatibilityRenderBackend(
		ApplicationConfig* application_config,
		Logger* logger,
		Uint32 sdl_only_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
	);
	/// sdl_window_flags must be set in every constructor to include `SDL_WINDOW_OPENGL`

//////////////////////
///// ATTRIBUTES /////
//////////////////////
};