# CMakeList.txt : CMake project for GameEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("GameEngine")

# Add source to this project's executable.
add_executable (runtime "src/runtime/main.cpp" "include/render_backends/compatibility/core.h" "include/render_backends/progressive/core.h" "src/render_backends/compatibility/core.cpp" "src/render_backends/progressive/core.cpp")
add_executable (editor "src/editor/main.cpp" "include/render_backends/compatibility/core.h" "include/render_backends/progressive/core.h" "src/render_backends/compatibility/core.cpp" "src/render_backends/progressive/core.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET runtime PROPERTY CXX_STANDARD 20)
  set_property(TARGET editor PROPERTY CXX_STANDARD 20)
endif()

# --- VULKAN DEPENDENCY ---

find_package(Vulkan REQUIRED)

# --- SDL2 DEPENDENCY ---

find_package(SDL2 CONFIG REQUIRED)

# --- ASSIMP DEPENDENCY ---
# If assimp fails to build (especially with vcpkg), make sure to install draco first

find_package(assimp CONFIG REQUIRED)

# --- GLM DEPENDENCY ---

find_package(glm CONFIG REQUIRED)

target_link_libraries(runtime PRIVATE SDL2::SDL2 SDL2::SDL2main Vulkan::Vulkan assimp::assimp glm::glm)
target_link_libraries(editor PRIVATE SDL2::SDL2 SDL2::SDL2main Vulkan::Vulkan assimp::assimp glm::glm)