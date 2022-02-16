##generate a static lib...
cmake_minimum_required(VERSION 3.16.3)

project(lei)

set(SOURCES
    src/server.cpp
    src/socket.cpp
    src/thread.cpp
    examples/echo.cpp
)
##add_executable(lei ${SOURCES})
add_library(lei STATIC 
    ${SOURCES}
)   

target_include_directories(lei  
    PRIVATE 
        src/*.h
)

target_link_libraries(lei 
    PRIVATE
        pthread
)