cmake_minimum_required(VERSION 3.7)

project(Renderer)

# Enable c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include OpenGL
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

# Include GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
include_directories(external/glfw/include)
include_directories(external/glad/include)
include_directories(external/glm)

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
target_link_libraries(Renderer ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} glfw)
