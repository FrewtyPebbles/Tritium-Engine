//*****************************************
// This is the entry point for the runtime
//*****************************************

#include <iostream>
#include <iomanip>

#include "Engine/constants.h"
#include "Engine/engine.h"
#include "Engine/logging/logger.h"
#include "Engine/thread_pool/thread_pool.h"
#include <stdexcept>

#ifdef RENDER_BACKEND_PROGRESSIVE
#include "Engine/render_backends/progressive/progressive_render_backend.h"
#endif // RENDER_BACKEND_PROGRESSIVE

#ifdef RENDER_BACKEND_COMPATIBILITY
#include "Engine/render_backends/compatibility/compatibility_render_backend.h"
#endif // RENDER_BACKEND_COMPATIBILITY

using std::cout, std::endl;

int main(int argc, char** argv)
{
	cout << "Runtime starting...  \n\n" << ENGINE_NAME << " Engine v" << GET_ENGINE_VERSION() << "\n" << endl;

	// TODO: Replace this code once game-data loading logic is available.
	// For now just create a test window.
	// Game-data loading logic will choose the render backend and load in the initial scene and node state.
	try {

		// Log pipes must be added to vector this way to avoid copying
		// since the file pointer in log pipes cant be copied
		vector<LogPipe*> logPipes;
		logPipes.reserve(2);

		LogPipe logPipeRendering("./.logs/rendering.log", "rendering");
		LogPipe logPipeUser("./.logs/user.log", "user");

		logPipes.push_back(&logPipeRendering);
		logPipes.push_back(&logPipeUser);

		Logger logger = Logger(logPipes);

		for (auto logPipe : logPipes) {
			cout << " - Logging to \"" << logPipe->name << "\"" << endl;
		}

		// This thread pool will be used on the backend and frontend to ensure
		// that we keep a global-ish count of the threads in use.

		ThreadPool::Pool threadPool = ThreadPool::Pool(5, std::thread::hardware_concurrency() / 2);
		
		// add 1 to the threadcount for the logger which has its own dedicated thread
		cout << " - Using " << threadPool.thread_count + 1 << " threads" << endl;

		// Create render backend
#ifdef RENDER_BACKEND_PROGRESSIVE
		ProgressiveRenderBackend renderBackend = ProgressiveRenderBackend(
			nullptr
		);

		cout << " - Using Progressive Render Backend" << endl;

#endif // RENDER_BACKEND_PROGRESSIVE

#ifdef RENDER_BACKEND_COMPATIBILITY
		CompatibilityRenderBackend renderBackend = CompatibilityRenderBackend(
			nullptr
		);
		
		cout << " - Using Compatibility Render Backend" << endl;

#endif // RENDER_BACKEND_COMPATIBILITY

		// Create an engine instance:

		Tritium::Engine engine = Tritium::Engine(
			&renderBackend,
			&logger,
			&threadPool,
			"TestApp",
			"This is a test app.",
			{ "Jane Doe" },
			0,
			0,
			0,
			"dev"
		);

		//start the window loop

		engine.start_window(engine.application_name, 600, 600);

		// after close window flush logger

		engine.logger->flush();

		cout << "\nRuntime stopped." << endl;
	}
	catch (std::runtime_error error) {
		std::cerr << "Fatal Error: " << error.what() << "\n";
	}

	return 0;
}
