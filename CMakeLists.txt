cmake_minimum_required(VERSION 3.0.0)
project(cpipe VERSION 0.1.0)

set(CMAKE_BUILD_TYPE Debug)

# Create a shared library
add_library(
    pipe_handler SHARED 
    source/pipe_handler.h
    source/pipe_handler.c
)

# Create executable 1
add_executable(writepipe source/writepipe.c)
target_link_libraries(writepipe pipe_handler)

# Create executable 2
add_executable(readpipe source/readpipe.c)
target_link_libraries(readpipe pipe_handler)

# Create executable 3
add_executable(write_ack source/write_ack.c)
target_link_libraries(write_ack pipe_handler)

# Create executable 4
add_executable(read_ack source/read_ack.c)
target_link_libraries(read_ack pipe_handler)
