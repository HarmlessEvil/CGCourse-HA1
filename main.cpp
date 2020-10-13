#pragma optimize("", off)

#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <GL/glew.h>

// Imgui + bindings
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// STB, load images
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include <stb_image.h>


// Math constant and routines for OpenGL interop
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl_shader.h"

#include "image.hpp"
#include "model.hpp"
#include "obj_model.h"
#include "terrain.hpp"

image load_image(std::string const &path, bool flip_vertically = true, int desired_channels = 0) {
    int width, height, channels;

    stbi_set_flip_vertically_on_load(flip_vertically);
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, desired_channels);

    if (desired_channels == 0) {
        channels = desired_channels;
    }

    if (!data) {
        throw std::runtime_error(stbi_failure_reason());
    }

    return image{static_cast<size_t>(width), static_cast<size_t>(height), channels, data, stbi_image_free};
}

GLuint load_texture(image const &image, GLenum format) {
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            image.width(),
            image.height(),
            0,
            format,
            GL_UNSIGNED_BYTE,
            image.data()
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
}

static void glfw_error_callback(int error, const char *description) {
    throw std::runtime_error(fmt::format("Glfw Error {}: {}\n", error, description));
}

void create_quad(GLuint &vbo, GLuint &vao, GLuint &ebo) {
    // create the triangle
    const float vertices[] = {
            -1, 1,
            0, 1,

            -1, -1,
            0, 0,

            1, -1,
            1, 0,

            1, 1,
            1, 1,
    };
    const unsigned int indices[] = {0, 1, 2, 0, 2, 3};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void create_terrain(terrain const &terrain, GLuint &vbo, GLuint &vao, GLuint &ebo) {
    // position: float3, normal: float3, tex_coord: float2
    std::vector<float> buffer{};

    for (std::size_t i = 0; i < terrain.height(); ++i) {
        for (std::size_t j = 0; j < terrain.width(); ++j) {
            glm::vec3 coordinate = terrain.coordinates()[i][j];
            buffer.push_back(coordinate.x);
            buffer.push_back(coordinate.y);
            buffer.push_back(coordinate.z);

            glm::vec3 normal = terrain.normals()[i][j];
            buffer.push_back(normal.x);
            buffer.push_back(normal.y);
            buffer.push_back(normal.z);

            glm::vec2 texture_coordinates = terrain.texture_coordinates()[i][j];
            buffer.push_back(texture_coordinates.x);
            buffer.push_back(texture_coordinates.y);
        }
    }

    std::vector<std::uint32_t> indices{};
    indices.reserve(terrain.quads_count() * 2 * 3);

    for (const auto &quad_row : terrain.quads()) {
        for (const auto &quad : quad_row) {
            for (const auto &triangle : quad) {
                for (const auto &index : triangle.indices) {
                    indices.push_back(index.first * terrain.width() + index.second);
                }
            }
        }
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), buffer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(std::uint32_t), indices.data(), GL_STATIC_DRAW);

    const GLsizei stride = 8 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

struct render_target_t {
    render_target_t(int res_x, int res_y);

    ~render_target_t();

    GLuint fbo_;
    GLuint color_, depth_;
    int width_, height_;
};

render_target_t::render_target_t(int res_x, int res_y) {
    width_ = res_x;
    height_ = res_y;

    glGenTextures(1, &color_);
    glBindTexture(GL_TEXTURE_2D, color_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, res_x, res_y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glGenTextures(1, &depth_);
    glBindTexture(GL_TEXTURE_2D, depth_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, res_x, res_y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
                 nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &fbo_);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           color_,
                           0);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           depth_,
                           0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer incomplete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

render_target_t::~render_target_t() {
    glDeleteFramebuffers(1, &fbo_);
    glDeleteTextures(1, &depth_);
    glDeleteTextures(1, &color_);
}


int main(int, char **) {
    image height_map = load_image("assets/textures/height_maps/fjord.png");
    terrain terrain{height_map, true};

    try {
        // Use GLFW to create a simple window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            throw std::runtime_error("glfwInit error");

        // GL 3.3 + GLSL 330
        const char *glsl_version = "#version 330";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

        // Create window with graphics context
        GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui - Conan", NULL, NULL);
        if (window == NULL)
            throw std::runtime_error("Can't create glfw window");

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        if (glewInit() != GLEW_OK)
            throw std::runtime_error("Failed to initialize glew");

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        auto bunny = create_model("assets/models/bunny.obj");
        render_target_t rt(512, 512);

        GLuint vbo, vao, ebo;
        create_terrain(terrain, vbo, vao, ebo);
//        create_quad(vbo, vao, ebo);

        // init shader
        shader_t terrain_shader(
                "assets/shaders/terrain.vs.glsl",
                "assets/shaders/terrain.fs.glsl"
        );
        shader_t quad_shader(
                "assets/shaders/simple-shader.vs",
                "assets/shaders/simple-shader.fs"
        );
        shader_t bunny_shader("assets/shaders/model.vs", "assets/shaders/model.fs");

        // Setup GUI context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui::StyleColorsDark();

        auto const start_time = std::chrono::steady_clock::now();

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // Get windows size
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);


            // Gui start new frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // GUI
            ImGui::Begin("Triangle Position/Color");
            static float rotation = 0.0;
            ImGui::SliderFloat("rotation", &rotation, 0, 2 * glm::pi<float>());
            static glm::vec3 translation = {0.0, 0.0, 0.0};
            ImGui::SliderFloat3("position", glm::value_ptr(translation), -1000.0, 1000.0);
            static glm::vec3 eye = {0, 0, -1};
            ImGui::SliderFloat3("eye", glm::value_ptr(eye), -1000.0f, 1000.0f);
            static bool wireframe = false;
            ImGui::Checkbox("wireframe", &wireframe);
            static int elements = 300;
            ImGui::SliderInt(
                    "elements",
                    &elements,
                    0,
                    static_cast<int>(terrain.quads_count() * 2 * 3) // 2 triangles, 3 vertices each in all quads
            );
            //static float color[4] = { 1.0f,1.0f,1.0f,1.0f };
            //ImGui::ColorEdit3("color", color);
            ImGui::End();

            float const time_from_start = (float) (
                    std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start_time).count() /
                    1000.0);


            // Render offscreen
//            {
//                auto model = glm::rotate(glm::mat4(1), glm::radians(time_from_start * 10), glm::vec3(0, 1, 0)) *
//                             glm::scale(glm::vec3(7, 7, 7));
//                auto view = glm::lookAt<float>(glm::vec3(0, 1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//                auto projection = glm::perspective<float>(90, float(rt.width_) / rt.height_, 0.1, 100);
//                auto mvp = projection * view * model;
//
//
//                glBindFramebuffer(GL_FRAMEBUFFER, rt.fbo_);
//                glViewport(0, 0, rt.width_, rt.height_);
//                glEnable(GL_DEPTH_TEST);
//                glColorMask(1, 1, 1, 1);
//                glDepthMask(1);
//                glDepthFunc(GL_LEQUAL);
//
//                glClearColor(0.3, 0.3, 0.3, 1);
//                glClearDepth(1);
//                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//                bunny_shader.use();
//                bunny_shader.set_uniform("u_mvp", glm::value_ptr(mvp));
//                bunny_shader.set_uniform("u_model", glm::value_ptr(model));
//
//                glm::vec3 light_dir = glm::rotateY(glm::vec3(1, 0, 0), glm::radians(time_from_start * 60));
//
//                bunny_shader.set_uniform<float>("u_color", 0.83, 0.64, 0.31);
//                bunny_shader.set_uniform<float>("u_light", light_dir.x, light_dir.y, light_dir.z);
//                bunny->draw();
//
//                glDisable(GL_DEPTH_TEST);
//                glBindFramebuffer(GL_FRAMEBUFFER, 0);
//            }

            // Render main
            {
                auto model = glm::rotate<float>(glm::translate(translation),
                                                rotation, //0.1 * (-1 + 2 * cos(time_from_start) * cos(time_from_start)),
                                                glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(1000, 1000, 255));
                auto view = glm::lookAt<float>(eye, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
                auto projection = glm::perspective<float>(90, float(display_w) / display_h, 0.1, 1000);
                auto mvp = projection * view * model;

                glViewport(0, 0, display_w, display_h);

                glEnable(GL_CULL_FACE);
                glEnable(GL_DEPTH_TEST);

                glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
                glDepthMask(1);
                glClearDepth(1);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                terrain_shader.use();
                terrain_shader.set_uniform("u_mvp", glm::value_ptr(mvp));

//                quad_shader.use();
//                quad_shader.set_uniform("u_mvp", glm::value_ptr(mvp));
//                quad_shader.set_uniform("u_tex", int(0));

                if (wireframe) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }

//                glActiveTexture(GL_TEXTURE0);
//                glBindTexture(GL_TEXTURE_2D, rt.color_);
//                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindVertexArray(vao);
                glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_INT, 0);
//                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);

                if (wireframe) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }

            // Generate gui render commands
            ImGui::Render();

            // Execute gui render commands using OpenGL backend
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Swap the backbuffer with the frontbuffer that is used for screen display
            glfwSwapBuffers(window);
        }

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }
    catch (std::exception const &e) {
        spdlog::critical("{}", e.what());
        return 1;
    }

    return 0;
}
