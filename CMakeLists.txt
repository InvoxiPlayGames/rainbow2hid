cmake_minimum_required(VERSION 3.10)

project(rainbow2hid)

file(GLOB SOURCE_FILES "source/*.c")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

find_package(hidapi REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE hidapi::hidapi)
target_include_directories(${PROJECT_NAME} PUBLIC "include" ${hidapi_INCLUDE_DIRS})
