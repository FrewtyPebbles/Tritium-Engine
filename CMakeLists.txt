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
add_executable (GameEngine "GameEngine.cpp" "GameEngine.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GameEngine PROPERTY CXX_STANDARD 20)
endif()

# --- VULKAN DEPENDENCY ---

find_package(Vulkan REQUIRED)

# --- SDL2 DEPENDENCY ---

find_package(SDL2 CONFIG REQUIRED)

target_link_libraries(GameEngine PRIVATE SDL2::SDL2 SDL2::SDL2main Vulkan::Vulkan)