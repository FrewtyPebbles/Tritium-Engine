#include "Engine/engine.h"
#include "Engine/render_backends/render_backend.h"
#include "Engine/logging/logger.h"

namespace Tritium {

	Engine::Engine(
		RenderBackend* render_backend,
		Logger* logger,
		ThreadPool::Pool* thread_pool,
		string application_name,
		string application_description,
		vector<string> application_authors,
		int application_version_major,
		int application_version_minor,
		int application_version_patch,
		string application_version_identifier
	)
		: render_backend(render_backend),
		logger(logger),
		thread_pool(thread_pool),
		application_name(application_name),
		application_description(application_description),
		application_authors(application_authors),
		application_version_major(application_version_major),
		application_version_minor(application_version_minor),
		application_version_patch(application_version_patch),
		application_version_identifier(application_version_identifier) {
		this->render_backend->engine = this;
	}

	void Engine::start_window(string window_title, uint32_t window_width, uint32_t window_height) {
		this->render_backend->start_window(window_title, window_width, window_height);
	}
	
};