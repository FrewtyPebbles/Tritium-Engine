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

# --- RUNTIME SOURCES ---

file(GLOB_RECURSE RUNTIME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
list(REMOVE_ITEM RUNTIME_SOURCES "${CMAKE_SOURCE_DIR}/src/editor/main.cpp")

# --- EDITOR SOURCES ---

file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
list(REMOVE_ITEM EDITOR_SOURCES "${CMAKE_SOURCE_DIR}/src/runtime/main.cpp")

# Add source to this project's executable.
add_executable (runtime ${RUNTIME_SOURCES})
add_executable (editor ${EDITOR_SOURCES})

target_include_directories(runtime PRIVATE include)
target_include_directories(editor PRIVATE include)

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