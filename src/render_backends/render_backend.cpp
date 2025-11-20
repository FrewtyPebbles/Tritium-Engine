// See .h file for comment explanations of === header === sections
#include "Engine/render_backends/render_backend.h"


// Simple functions like getters and setters go at the bottom.
// Organize from most complex at the top to least complex at the bottom.

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