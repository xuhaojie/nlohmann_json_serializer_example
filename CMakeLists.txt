cmake_minimum_required(VERSION 3.10)
project(json_serializer_example)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 寻找nlohmann/json库（优先系统库，其次本地源码）
find_package(nlohmann_json QUIET)

if(nlohmann_json_FOUND)
    message(STATUS "Found system nlohmann/json: ${nlohmann_json_VERSION}")
else()
    message(STATUS "System nlohmann/json not found, using local source")
    # 添加本地头文件（假设third_party/nlohmann/json.hpp存在）
    include_directories(third_party)
endif()

# 添加可执行文件
add_executable(json_example src/main.cpp)

# 链接系统库（若找到）
if(nlohmann_json_FOUND)
    target_link_libraries(json_example PRIVATE nlohmann_json::nlohmann_json)
endif()
