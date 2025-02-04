
cmake_minimum_required(VERSION 3.21.3)

if(NOT DEFINED VCPKG_ROOT)
    set(VCPKG_ROOT "C:/Users/arsbo/vcpkg")
endif()

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project(vulkan)

set(SOURCES 
    sources/main/main.cpp 
    sources/renderer/vk_renderer.cpp)

find_package(PkgConfig REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
