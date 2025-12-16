#pragma once

#define SDL_MAIN_HANDLED

#include <functional>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "Engine/logging/logger.h"

using std::string;
using std::vector;

// --- RenderBackend --- 
// This class is derived to create the progressive and compatibility render backends.
// ---------------------

class RenderBackend {

public:

/////////////////////
///// FUNCTIONS /////
/////////////////////

// ==== Class Functions ====
// These include things like constructors, destructors and operators.
// ---

	RenderBackend(
		string application_name,
		string application_description,
		vector<string> application_authors,
		int application_version_major,
		int application_version_minor,
		int application_version_patch,
		string application_version_identifier,
		Logger* logger
	) : application_name(application_name),
		application_description(application_description),
		application_authors(application_authors),
		application_version_major(application_version_major),
		application_version_minor(application_version_minor),
		application_version_patch(application_version_patch),
		application_version_identifier(application_version_identifier),
		logger(logger)
	{};
	/// default settings?

// ==== Window Functions ====
// These functions are related to the SDL window that the backend renders onto.
// Some of these may be implemented in this base RenderBackend class.
// ---

	bool start_window(string window_title, int window_width, int window_height);
	/// Returns false on failure, check cerr.  Maybe prompt the user to make a github issue.

	void close_window();


// === Game Loop Functions ===
// These are functions related to the game loop.
// Includes game loop hook functions like "_on_update(float deltaTime)"
// ---

	// = Time / Time Scale Functions = 
	// These are for getting and setting different time / time scale parameters.
	// --
	
	float get_delta_time();
	long int get_time_seconds();
	long int get_time_milliseconds();
	long int get_time_nanoseconds();
	void set_fixed_update_ticks_per_second(int ticksPerSecond);
	/// sets the amount of updates per second.


	// = Callback Functions = 
	// These functions are for adding hooks into the render loop to the scripts attached to Nodes.
	// --

	void add_on_draw_update_callback(int nodeID, std::function<void(float)> callback);
	/// Callback includes delta_time as an argument.

	void remove_on_draw_update_callback(int nodeID);
	/// Callback includes delta_time as an argument.

	void add_on_fixed_update_callback(int nodeID, std::function<void()> callback);
	/// Fixed update is an update loop that runs every game tick
	// mostly used for physics updates

	void remove_on_fixed_update_callback(int nodeID);
	/// Fixed update is an update loop that runs every game tick
	// mostly used for physics updates

//////////////////////
///// ATTRIBUTES /////
//////////////////////

protected:

/////////////////////
///// FUNCTIONS /////
/////////////////////

// === Window Functions ===

	virtual void before_start_window(string window_title, int window_width, int window_height) = 0;
	/// This runs at the start of `start_window` incase anything needs to be initialized in derived classes.

// === Game Loop ===

	virtual void before_game_loop() = 0;
	/// This runs before the game while loop start in `start_game_loop`
	// this is where we will initialize vulkan stuff.
	// This will be implemented in derived classes.

	virtual void after_game_loop() = 0;
	/// This runs after the game while loop ends in `start_game_loop`
	// this is where we will deinitialize vulkan stuff or clean it up.
	// This will be implemented in derived classes.

	virtual void update_game() = 0;
	/// Runs inside "start_game_loop" for each frame.
	// Updates the scene and calls all relevant hooks such as "on_draw_update".
	// This will be implemented in derived classes.
	// It may call functions from the base class for shared logic between
	// render backends.

//////////////////////
///// ATTRIBUTES /////
//////////////////////

// === SDL2 ===

	// = Window =
	// everything related to window management
	// --

	SDL_Window* sdl_window;

	Uint32 sdl_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;
	/// This will be set to something different depending on the graphics API(s) that the backend uses.

// === Game Loop ===

	bool window_running = false;

// === Metadata ===
	
	string application_name;
	string application_description;
	vector<string> application_authors;
	int application_version_major;
	int application_version_minor;
	int application_version_patch;
	string application_version_identifier;

// === Logging ===

	Logger* logger;

private:

/////////////////////
///// FUNCTIONS /////
/////////////////////

// === Time / Draw Loops ===

	void start_game_loop();
	/// Initializes the game state and starts the game loop which calls update_game.
	// Loads in the initial scene file.

	void SDL_forward_event(SDL_Event event);
	/// This forwards `SDL_Event`s to their hooks or wherever they need to go.

	void start_fixed_update_game_loop();
	/// Initializes the fixed update loop and starts it, calling "fixed_update_game" every
	// itteration.

	void fixed_update_game();
	/// Runs inside the fixed update loop.  Updates physics and other systems.  Also calls
	// relevant hooks such as "on_fixed_update".

// === Callbacks ===

	void execute_on_draw_update_callbacks();

	void execute_on_fixed_update_callbacks();

//////////////////////
///// ATTRIBUTES /////
//////////////////////

// === Callbacks ===

	std::unordered_map<int, std::function<void(float)>> on_draw_update_callbacks;
	std::unordered_map<int, std::function<void()>> on_fixed_update_callbacks;

// === Time / Time Scales ===

	float delta_time;
	long int time_seconds;
	long int time_milliseconds;
	long int time_nanoseconds;
	int fixed_update_ticks_per_second;

// === SDL2 ===

	// = Events =
	SDL_Event sdl_event;

};
