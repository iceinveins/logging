cmake_minimum_required(VERSION 3.1)

project (Logging)

add_definitions("-Wall -Wextra")
add_subdirectory(logIf)

add_executable(Logging logserver.cpp)
target_link_libraries(Logging PUBLIC logInterface)
target_link_libraries(Logging PUBLIC librt.so)
target_include_directories(Logging PUBLIC "logIf/interface/include")

add_executable(ClientDemo demo.cpp)
target_link_libraries(ClientDemo PUBLIC logAgent)
target_link_libraries(ClientDemo PUBLIC logInterface)
target_link_libraries(ClientDemo PUBLIC librt.so)
target_include_directories(ClientDemo PUBLIC "logIf/interface/include")
target_include_directories(ClientDemo PUBLIC "logIf/agent/include")
