cmake_minimum_required(VERSION 3.0)
project(opengl-imgui-sample CXX)

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# CONFIG option is important so that CMake doesnt search for modules into the default modules directory
find_package(imgui CONFIG)
find_package(glfw CONFIG)
find_package(glew CONFIG)
find_package(fmt CONFIG)
find_package(glm CONFIG)
find_package(stb CONFIG)
find_package(tinyobjloader CONFIG)
find_package(spdlog CONFIG)

set(_src_root_path "${PROJECT_SOURCE_DIR}")

add_executable(opengl-imgui-sample
        main.cpp
        obj_model.cpp
        obj_model.h
        opengl_shader.cpp
        opengl_shader.h
        bindings/imgui_impl_glfw.cpp
        bindings/imgui_impl_opengl3.cpp
        bindings/imgui_impl_glfw.h
        bindings/imgui_impl_opengl3.h
        image.cpp
        rectangular_model.cpp
        terrain.cpp
        toric_terrain.cpp)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${PROJECT_BINARY_DIR})

set_target_properties(opengl-imgui-sample PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET opengl-imgui-sample PROPERTY CXX_STANDARD 17)

target_compile_definitions(opengl-imgui-sample PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(opengl-imgui-sample imgui::imgui GLEW::glew_s glfw::glfw fmt::fmt glm::glm stb::stb tinyobjloader::tinyobjloader spdlog::spdlog)
