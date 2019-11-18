cmake_minimum_required(VERSION 3.7)

project(Renderer)

# Enable c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Include OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# Include GLUT
if(WIN32)
    set(GLUT_INCLUDE_DIR "external/glut/include")
    set(OPENGL_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/external/glut/lib")
    # set(GLUT_LIBRARY "${CMAKE_SOURCE_DIR}/external/glut/lib/x64/freeglut")
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIR})
else(WIN32)
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIRS})
endif(WIN32)

# Include GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
include_directories(external/glfw/include)
include_directories(external/glad/include)
include_directories(external/glm/include)

# Include Box2D
add_subdirectory(external/box2d)
include_directories(external/box2d)

# Include main project
file(GLOB RENDERER_SRC
    "include/*.h"
    "src/*.cpp"
    "src/*/*.cpp"
    "external/glad/src/glad.c"
)

include_directories("include")

add_executable(Renderer ${RENDERER_SRC})

# Link the libraries
target_link_libraries(Renderer ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} glfw Box2D)

# Copy shaders
add_custom_command(TARGET Renderer POST_BUILD    # Adds a post-build event to the project
        COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_directory..."
        "${CMAKE_SOURCE_DIR}/shaders"              # <--this is in-file
        "$<TARGET_FILE_DIR:Renderer>/shaders")             # <--this is out-file path
