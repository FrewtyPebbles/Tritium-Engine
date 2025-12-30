# CMakeList.txt : CMake project for GameEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# *** BUILD FLAGS ***

set(RENDER_BACKEND "progressive" CACHE STRING "Choose the render backend: progressive/compatibility")

if(RENDER_BACKEND STREQUAL "progressive")
    add_compile_definitions(RENDER_BACKEND_PROGRESSIVE)
elseif(RENDER_BACKEND STREQUAL "compatibility")
    add_compile_definitions(RENDER_BACKEND_COMPATIBILITY)
endif()

# --- functions ---

function(copy_spv_files TARGET ROOT_DIR)
    file(GLOB_RECURSE SPV_FILES "${ROOT_DIR}/*.spv")
    foreach(SPV ${SPV_FILES})
        file(RELATIVE_PATH REL_PATH ${ROOT_DIR} ${SPV})
        set(DEST "$<TARGET_FILE_DIR:${TARGET}>/shaders/${REL_PATH}")
        get_filename_component(DEST_DIR ${DEST} DIRECTORY)
        add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${DEST_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SPV} ${DEST}
        )
    endforeach()
endfunction()

# Recursively add include directories except ignored ones
function(target_include_except target root_dir)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs IGNORES)
    cmake_parse_arguments(TIE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Get all subdirectories recursively
    file(GLOB_RECURSE ALL_ITEMS "${root_dir}/*")
    set(ALL_DIRS "${root_dir}")
    
    foreach(ITEM ${ALL_ITEMS})
        if(IS_DIRECTORY "${ITEM}")
            list(APPEND ALL_DIRS "${ITEM}")
        endif()
    endforeach()
    
    # Remove duplicates
    list(REMOVE_DUPLICATES ALL_DIRS)
    
    # Filter out ignored directories
    set(INCLUDE_DIRS "")
    foreach(DIR ${ALL_DIRS})
        file(RELATIVE_PATH REL_DIR "${root_dir}" "${DIR}")
        
        # Check if this directory matches any ignore pattern
        set(SHOULD_IGNORE FALSE)
        foreach(IGNORE ${TIE_IGNORES})
            # Check if the relative path starts with the ignore pattern
            string(FIND "${REL_DIR}" "${IGNORE}" MATCH_POS)
            if(MATCH_POS EQUAL 0)
                set(SHOULD_IGNORE TRUE)
                break()
            endif()
        endforeach()
        
        if(NOT SHOULD_IGNORE)
            list(APPEND INCLUDE_DIRS "${DIR}")
        endif()
    endforeach()
    
    # Add all collected directories to target
    target_include_directories(${target} PRIVATE ${INCLUDE_DIRS})
endfunction()


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# --- SET PROJECT NAME ---

project ("GameEngine")

# --- RUNTIME SOURCES ---

file(GLOB_RECURSE RUNTIME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

list(REMOVE_ITEM RUNTIME_SOURCES
    "${CMAKE_SOURCE_DIR}/src/editor/main.cpp"
)

# Glob remove files for other render backend

if(RENDER_BACKEND STREQUAL "progressive")
    list(FILTER RUNTIME_SOURCES EXCLUDE REGEX "render_backends/compatibility/.*\\.cpp")
elseif(RENDER_BACKEND STREQUAL "compatibility")
    list(FILTER RUNTIME_SOURCES EXCLUDE REGEX "render_backends/progressive/.*\\.cpp")
endif()

# --- EDITOR SOURCES ---

file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
list(REMOVE_ITEM EDITOR_SOURCES
    "${CMAKE_SOURCE_DIR}/src/runtime/main.cpp"
)

# Glob remove files for other render backend

if(RENDER_BACKEND STREQUAL "progressive")
    list(FILTER EDITOR_SOURCES EXCLUDE REGEX "render_backends/compatibility/.*\\.cpp")
elseif(RENDER_BACKEND STREQUAL "compatibility")
    list(FILTER EDITOR_SOURCES EXCLUDE REGEX "render_backends/progressive/.*\\.cpp")
endif()

# --- CREATE EXECUTABLE TARGETS ---

add_executable (runtime ${RUNTIME_SOURCES})
add_executable (editor ${EDITOR_SOURCES})

# --- ADD INCLUDE DIRECTORY ---

if(RENDER_BACKEND STREQUAL "progressive")
    target_include_except(runtime "${CMAKE_SOURCE_DIR}/include" IGNORES "Engine/render_backends/compatibility")
    target_include_except(editor "${CMAKE_SOURCE_DIR}/include" IGNORES "Engine/render_backends/compatibility")
elseif(RENDER_BACKEND STREQUAL "compatibility")
    target_include_except(runtime "${CMAKE_SOURCE_DIR}/include" IGNORES "Engine/render_backends/progressive")
    target_include_except(editor "${CMAKE_SOURCE_DIR}/include" IGNORES "Engine/render_backends/progressive")
endif()

# --- SET C++ STANDARD TO C++ 20 ---

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET runtime PROPERTY CXX_STANDARD 20)
  set_property(TARGET editor PROPERTY CXX_STANDARD 20)
endif()

# --- VULKAN DEPENDENCY ---

if(RENDER_BACKEND STREQUAL "progressive")
    find_package(Vulkan REQUIRED)
endif()

# --- SDL2 DEPENDENCY ---

find_package(SDL2 CONFIG REQUIRED)

# --- ASSIMP DEPENDENCY ---
# If assimp fails to build (especially with vcpkg), make sure to install draco first

find_package(assimp CONFIG REQUIRED)

# --- GLM DEPENDENCY ---

find_package(glm CONFIG REQUIRED)

# --- LINK DEPENDENCIES ---

if(RENDER_BACKEND STREQUAL "progressive")
    target_link_libraries(runtime PRIVATE
        SDL2::SDL2 Vulkan::Vulkan
        assimp::assimp glm::glm
    )
    target_link_libraries(editor PRIVATE
        SDL2::SDL2 Vulkan::Vulkan
        assimp::assimp glm::glm
    )
elseif(RENDER_BACKEND STREQUAL "compatibility")
    # TODO : ADD OPENGL dependencies
    target_link_libraries(runtime PRIVATE
        SDL2::SDL2
        assimp::assimp glm::glm
    )
    target_link_libraries(editor PRIVATE
        SDL2::SDL2
        assimp::assimp glm::glm
    )
endif()

# --- COPY SHADERS ---

if(RENDER_BACKEND STREQUAL "progressive")
    copy_spv_files(runtime "${CMAKE_SOURCE_DIR}/src/render_backends/progressive/shaders")
    copy_spv_files(editor "${CMAKE_SOURCE_DIR}/src/render_backends/progressive/shaders")
elseif(RENDER_BACKEND STREQUAL "compatibility")
    copy_spv_files(runtime "${CMAKE_SOURCE_DIR}/src/render_backends/compatibility/shaders")
    copy_spv_files(editor "${CMAKE_SOURCE_DIR}/src/render_backends/progressive/shaders")
endif()
