cmake_minimum_required(VERSION 3.27)
project(VulkanLearn)

set(CMAKE_CXX_STANDARD 17)

add_executable(VulkanLearn source/main.cpp
        source/vulkan/Device.hpp
        source/vulkan/Device.cpp
        source/vulkan/VulkanCommon.h
        source/vulkan/Window.cpp
        source/vulkan/Window.h
        source/vulkan/Pipeline.cpp
        source/vulkan/Pipeline.h
        source/vulkan/Application.cpp
        source/vulkan/Application.h
        source/vulkan/SwapChain.hpp
        source/vulkan/SwapChain.cpp
        source/vulkan/Model.cpp
        source/vulkan/Model.h
        source/engine/Object.cpp
        source/engine/Object.h
        source/vulkan/Renderer.cpp
        source/vulkan/Renderer.h
        source/vulkan/RenderSystem.cpp
        source/vulkan/RenderSystem.h
        source/vulkan/Camera.cpp
        source/vulkan/Camera.h
        source/engine/MovementController.cpp
        source/engine/MovementController.h
        source/vulkan/renderingutility.h
        include/tiny_obj_loader.h
)

find_package(vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

add_subdirectory(include/glfw-3.3.9)
target_link_libraries(${PROJECT_NAME} glfw)

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
    set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
else()
    set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslangValidator.exe")
endif()

file(GLOB_RECURSE GLSL_SOURCE_FILES
        "shaders/*.frag"
        "shaders/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(${PROJECT_NAME} Shaders)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/shaders/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/shaders"
        "${PROJECT_SOURCE_DIR}/shaders"
)