cmake_minimum_required(VERSION 4.0)
project(zsh_db_engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create a library or executable target
add_library(sqlite3_ops)

# Declare module interface units (.ixx, .cppm, etc.)
target_sources(sqlite3_ops
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
        dbwrapper.cppm
        PRIVATE
        dbwrapper.cpp
)

# Executable consuming the module
add_executable(zsh_db_engine
        engine.cpp
)

target_link_libraries(zsh_db_engine
        PRIVATE sqlite3_ops
)