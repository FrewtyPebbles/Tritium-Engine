//*****************************************
// This is the entry point for the runtime
//*****************************************

#include <iostream>
#include <iomanip>
#include "Engine/render_backends/progressive/progressive_render_backend.h"
#include "Engine/constants.h"
#include <stdexcept>
#include "Engine/logging/logger.h"

using std::cout, std::endl;

int main(int argc, char** argv)
{
	cout << "Runtime starting...   v" << GET_ENGINE_VERSION() << endl;

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

		// Create userconfig

		ApplicationConfig applicationConfig = ApplicationConfig(
			"TestApp",
			"This is a test app.",
			{ "Jane Doe" },
			0,
			0,
			0,
			"dev"
		);

		// Create/start backend

		ProgressiveRenderBackend backend = ProgressiveRenderBackend(
			&applicationConfig,
			&logger
		);

		backend.start_window(applicationConfig.application_name, 600, 600);
	}
	catch (std::runtime_error error) {
		std::cerr << "Fatal Error: " << error.what() << "\n";
	}

	return 0;
}
