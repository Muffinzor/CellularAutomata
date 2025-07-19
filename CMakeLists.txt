cmake_minimum_required(VERSION 3.10)
project(CellularAutomata)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)

# SFML includes
include_directories(${CMAKE_SOURCE_DIR}/SFML/include)
link_directories(${CMAKE_SOURCE_DIR}/SFML/lib)

# Recursively find all .cpp files, excluding build folders
file(GLOB_RECURSE SOURCES
        RELATIVE ${CMAKE_SOURCE_DIR}
        "*.cpp"
)

# Filter out unwanted files
list(FILTER SOURCES EXCLUDE REGEX "^build/")
list(FILTER SOURCES EXCLUDE REGEX "^cmake-build-debug/")

message(STATUS "Your source files: ${SOURCES}")

add_executable(CellularAutomata ${SOURCES}
        SourceCode/Grid/CellularMatrix.cpp
        SourceCode/Grid/CellularMatrix.h
        SourceCode/Particles/Particle.cpp
        SourceCode/Particles/Particle.h
        SourceCode/Grid/Chunk.cpp
        SourceCode/Grid/Chunk.h
        SourceCode/InputManager.cpp
        SourceCode/InputManager.h
        SourceCode/Particles/SandParticle.h
        SourceCode/Particles/StoneParticle.h
        SourceCode/Particles/ParticleBehavior.cpp
        SourceCode/Particles/ParticleBehavior.h
        SourceCode/ThreadPool.cpp
        SourceCode/ThreadPool.h)

target_link_libraries(CellularAutomata
        sfml-graphics
        sfml-window
        sfml-system
)