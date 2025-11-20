#pragma once

#include <functional>
#include <unordered_map>

// --- RenderBackend --- 
// This class is derived to create the progressive and compatibility render backends.
// ---------------------

class RenderBackend {

public:

/////////////////////
///// FUNCTIONS /////
/////////////////////

// ==== Window Functions ====
// These functions are related to the SDL window that the backend renders onto.
// Some of these may be implemented in this base RenderBackend class.
// ---

	void init_window(int width, int height);
	void close_window();
	
// === Game Loop Functions ===
// These are functions related to the game loop.
// Includes game loop hook functions like "_on_update(float deltaTime)"
// ---

	
	float get_delta_time();
	long int get_time_seconds();
	long int get_time_milliseconds();
	long int get_time_nanoseconds();


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

	void set_fixed_update_ticks_per_second(int ticksPerSecond);
	/// sets the amount of updates per second.

//////////////////////
///// ATTRIBUTES /////
//////////////////////

private:

/////////////////////
///// FUNCTIONS /////
/////////////////////

	void start_game_loop();
	/// Initializes the game state and starts the game loop which calls update_game.
	// Loads in the initial scene file.

	void update_game();
	/// Runs inside "start_game_loop" for each frame.
	// Updates the scene and calls all relevant hooks such as "on_draw_update".

	void start_fixed_update_game_loop();
	/// Initializes the fixed update loop and starts it, calling "fixed_update_game" every
	// itteration.

	void fixed_update_game();
	/// Runs inside the fixed update loop.  Updates physics and other systems.  Also calls
	// relevant hooks such as "on_fixed_update".

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

};