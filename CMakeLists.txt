cmake_minimum_required(VERSION 3.10)
project(LockfreeQueue)

set(CMAKE_CXX_STANDARD 20)


set(CMAKE_BUILD_TYPE Debug)


include_directories(include)


if(MSVC)
    
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od /W4") 
    set(CMAKE_LINK_FLAGS_DEBUG "${CMAKE_LINK_FLAGS_DEBUG} /DEBUG")    
else()
    
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()


add_executable(test_queue src/test_queue.cpp)

add_executable(test_linked_list src/test_linked_list.cpp)

add_executable(test_priority_queue src/test_priority_queue.cpp)

if(NOT MSVC)
    target_link_libraries(test_queue asan)
    target_link_libraries(test_linked_list asan)
    target_link_libraries(test_priority_queue asan)
    
endif()
