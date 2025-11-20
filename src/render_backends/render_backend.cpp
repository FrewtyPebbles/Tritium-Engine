#include "Engine/render_backends/render_backend.h"

void RenderBackend::add_on_draw_update_callback(int nodeID, std::function<void(float)> callback) {
	this->on_draw_update_callbacks.insert_or_assign(nodeID, callback);
}

void RenderBackend::remove_on_draw_update_callback(int nodeID) {
	this->on_draw_update_callbacks.erase(nodeID);
}

void RenderBackend::add_on_fixed_update_callback(int nodeID, std::function<void(float)> callback) {
	this->on_draw_update_callbacks.insert_or_assign(nodeID, callback);
}

void RenderBackend::remove_on_fixed_update_callback(int nodeID) {
	this->on_draw_update_callbacks.erase(nodeID);
}

void RenderBackend::set_fixed_update_ticks_per_second(int ticksPerSecond) {
	this->fixed_update_ticks_per_second = ticksPerSecond;
}