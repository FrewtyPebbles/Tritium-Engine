// See .h file for comment explanations of === header === sections
#include "Engine/render_backends/render_backend.h"
#include <iostream>

// CODE FORMATTING INFORMATION:
// Simple functions like getters and setters go at the bottom.
// Organize from most complex at the top to least complex at the bottom.
// Unless it makes more sense to put a specific function above another.

bool RenderBackend::start_window(string window_title, int window_width, int window_height) {
	// This function is the function which starts the end user's application.
	
	// === Start SDL2 ===
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
		return false;
	}

	this->sdl_window = SDL_CreateWindow(
		window_title.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
	);

	if (!this->sdl_window) {
		std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
		SDL_Quit();
		return false;
	}

	// === Start the loop that renders the game ===
	this->start_game_loop();

	SDL_DestroyWindow(this->sdl_window);
	
	SDL_Quit();

	return true;
}

void RenderBackend::start_game_loop() {

	// Initialize render backend api
	this->before_game_loop();

	Uint32 lastTime = SDL_GetTicks();

	// Start loop
	this->window_running = true;
	while (this->window_running) {
		
		// Get delta_time
		
		Uint32 currentTime = SDL_GetTicks();
		
		this->delta_time = (currentTime - lastTime) / 1000.0f; // seconds
		
		lastTime = currentTime;
		
		// SDL Event Handling
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			this->SDL_event_handler(event);
		}

		this->execute_on_draw_update_callbacks();
		
		// Update the game
		this->update_game();
	}
	
	// Deinitialize render backend api
	this->after_game_loop();
}

// !!! TEMP FUNCTIONS SO I CAN BUILD !!!
void RenderBackend::SDL_event_handler(SDL_Event){}
void RenderBackend::execute_on_draw_update_callbacks(){}
// !!! TEMP FUNCTIONS SO I CAN BUILD !!!

// === Callback Functions ===

void RenderBackend::add_on_draw_update_callback(int nodeID, std::function<void(float)> callback) {
	this->on_draw_update_callbacks.insert_or_assign(nodeID, callback);
}

void RenderBackend::remove_on_draw_update_callback(int nodeID) {
	this->on_draw_update_callbacks.erase(nodeID);
}

void RenderBackend::add_on_fixed_update_callback(int nodeID, std::function<void()> callback) {
	this->on_fixed_update_callbacks.insert_or_assign(nodeID, callback);
}

void RenderBackend::remove_on_fixed_update_callback(int nodeID) {
	this->on_fixed_update_callbacks.erase(nodeID);
}

// === Time / Time Scale ===

float RenderBackend::get_delta_time() {
	return this->delta_time;
}
long int RenderBackend::get_time_seconds() {
	return this->time_seconds;
}
long int RenderBackend::get_time_milliseconds() {
	return this->time_milliseconds;
}
long int RenderBackend::get_time_nanoseconds() {
	return this->time_nanoseconds;
}

void RenderBackend::set_fixed_update_ticks_per_second(int ticksPerSecond) {
	this->fixed_update_ticks_per_second = ticksPerSecond;
}
