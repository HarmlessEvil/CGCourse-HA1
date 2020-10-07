#pragma optimize("", off)

#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>
#include <unordered_map>

#include <fmt/format.h>

#include <GL/glew.h>

// Imgui + bindings
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// STB, load images
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

// Tiny .obj loader
#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>

// Math constant and routines for OpenGL interop
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "opengl_shader.h"

static void glfw_error_callback(int error, const char *description) {
    std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

GLuint generate_default_texture() {
    GLuint texture;

    unsigned char default_texture[3] = {255, 255, 255};

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, default_texture);

    return texture;
}

GLuint load_cubemap(std::vector<std::string> const &faces) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, texture);

    for (std::size_t i = 0; i < faces.size(); ++i) {
        int width, height, channels;
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &channels, STBI_rgb);

        if (!data) {
            std::cerr << "Failed to load cubemap: " << faces[i] << std::endl;
            stbi_image_free(data);
            exit(1);
        }

        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB, GL_UNSIGNED_BYTE,
                data
        );
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}

void load_image(std::string const &path, GLuint &texture) {
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *image = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);
}

struct skybox {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    std::size_t triangles_count;
};

struct drawable {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    std::size_t material_id;

    std::size_t triangles_count;
};

skybox load_skybox() {
    float vertices[] = {
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
    };
    unsigned int indices[] = {
            5, 7, 3, 3, 1, 5,
            5, 4, 7, 7, 4, 6,
            6, 2, 7, 7, 2, 3,
            1, 3, 0, 0, 3, 2,
            0, 2, 4, 2, 6, 4,
            0, 5, 1, 0, 4, 5
    };

    skybox skybox{};

    glGenVertexArrays(1, &skybox.vao);
    glGenBuffers(1, &skybox.vbo);
    glGenBuffers(1, &skybox.ebo);
    glBindVertexArray(skybox.vao);

    glBindBuffer(GL_ARRAY_BUFFER, skybox.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    skybox.triangles_count = sizeof(indices) / sizeof(unsigned int) / 3;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return skybox;
}

// Loads model from .obj file. Assumes that if the model uses some material defined in .mtl file, the material is stored
// in exact same directory as the .obj file itself.
void load_model(
        std::vector<drawable> &objects_to_draw,
        std::vector<tinyobj::material_t> &materials,
        std::unordered_map<std::string, GLuint> &textures,
        std::filesystem::path const &path
) {
    tinyobj::attrib_t attrib;

    std::vector<tinyobj::shape_t> shapes;

    std::string err;

    std::string base_dir = path.parent_path();
    base_dir += std::filesystem::path::preferred_separator;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(), base_dir.c_str());

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    // Append default material
    materials.emplace_back();

    for (const auto &material : materials) {
        std::string texture_filename = material.diffuse_texname;

        if (texture_filename.empty() || textures.find(texture_filename) != textures.end()) {
            continue;
        }

        if (!std::filesystem::exists(texture_filename)) {
            texture_filename.insert(0, base_dir);
            if (!std::filesystem::exists(texture_filename)) {
                std::cerr << "Could not find texture: " << texture_filename << std::endl;
                exit(1);
            }
        }

        GLuint texture_id;
        load_image(texture_filename, texture_id);

        textures.emplace(material.diffuse_texname, texture_id);
    }

    for (const auto &shape : shapes) {
        auto &mesh = shape.mesh;

        // position: float3, normal: float3, ambient: float3, diffuse: float3, texCoords: float2
        std::vector<float> buffer;

        for (std::size_t f = 0; f < mesh.indices.size() / 3; ++f) {
            tinyobj::index_t idx0 = mesh.indices[3 * f + 0];
            tinyobj::index_t idx1 = mesh.indices[3 * f + 1];
            tinyobj::index_t idx2 = mesh.indices[3 * f + 2];

            float texture_coords[3][2];
            texture_coords[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
            texture_coords[0][1] = attrib.texcoords[2 * idx0.texcoord_index + 1];

            texture_coords[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
            texture_coords[1][1] = attrib.texcoords[2 * idx1.texcoord_index + 1];

            texture_coords[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
            texture_coords[2][1] = attrib.texcoords[2 * idx2.texcoord_index + 1];

            float v[3][3];
            for (std::size_t k = 0; k < 3; ++k) {
                int f0 = idx0.vertex_index;
                int f1 = idx1.vertex_index;
                int f2 = idx2.vertex_index;

                v[0][k] = attrib.vertices[3 * f0 + k];
                v[1][k] = attrib.vertices[3 * f1 + k];
                v[2][k] = attrib.vertices[3 * f2 + k];
            }

            float n[3][3];
            for (std::size_t k = 0; k < 3; ++k) {
                int nf0 = idx0.normal_index;
                int nf1 = idx1.normal_index;
                int nf2 = idx2.normal_index;

                n[0][k] = attrib.normals[3 * nf0 + k];
                n[1][k] = attrib.normals[3 * nf1 + k];
                n[2][k] = attrib.normals[3 * nf2 + k];
            }

            int material_id = mesh.material_ids[f];
            auto diffuse = materials[material_id].diffuse;
            auto ambient = materials[material_id].ambient;

            for (std::size_t k = 0; k < 3; ++k) {
                buffer.push_back(v[k][0]);
                buffer.push_back(v[k][1]);
                buffer.push_back(v[k][2]);

                buffer.push_back(n[k][0]);
                buffer.push_back(n[k][1]);
                buffer.push_back(n[k][2]);

                buffer.push_back(ambient[0]);
                buffer.push_back(ambient[1]);
                buffer.push_back(ambient[2]);

                buffer.push_back(diffuse[0]);
                buffer.push_back(diffuse[1]);
                buffer.push_back(diffuse[2]);

                buffer.push_back(texture_coords[k][0]);
                buffer.push_back(texture_coords[k][1]);
            }
        }

        drawable object{};
        object.material_id = mesh.material_ids[0];

        glGenVertexArrays(1, &object.vao);
        glGenBuffers(1, &object.vbo);
        glGenBuffers(1, &object.ebo);
        glBindVertexArray(object.vao);

        glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
        glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), buffer.data(), GL_STATIC_DRAW);

        object.triangles_count = buffer.size() / (3 + 3 + 3 + 3 + 2) / 3;
        std::vector<unsigned int> indices(object.triangles_count * 3);
        for (std::size_t i = 0; i < indices.size(); ++i) {
            indices[i] = i;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        const GLsizei stride = (3 + 3 + 3 + 3 + 2) * sizeof(float);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void *) (6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void *) (9 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, stride, (void *) (12 * sizeof(float)));
        glEnableVertexAttribArray(4);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        objects_to_draw.push_back(object);
    }
}

int main(int, char **) {
    // Use GLFW to create a simple window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;


    // GL 3.3 + GLSL 330
    const char *glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(720, 720, "Dear ImGui - Conan", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW, i.e. fill all possible function pointers for current OpenGL context
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return 1;
    }

    GLuint cubemap_texture = load_cubemap({
                                                  "assets/textures/skybox/right.jpg",
                                                  "assets/textures/skybox/left.jpg",
                                                  "assets/textures/skybox/top.jpg",
                                                  "assets/textures/skybox/bottom.jpg",
                                                  "assets/textures/skybox/front.jpg",
                                                  "assets/textures/skybox/back.jpg"
                                          });
    skybox skybox = load_skybox();

    std::vector<drawable> objects_to_draw{};
    std::vector<tinyobj::material_t> materials{};

    std::unordered_map<std::string, GLuint> textures{};
    GLuint default_texture = generate_default_texture();

    load_model(objects_to_draw, materials, textures, std::filesystem::path("assets/models/penguin/penguin.obj"));

    // init shader
    shader_t triangle_shader(
            "assets/shaders/simple-shader.vs",
            "assets/shaders/simple-shader.fs"
    );
    shader_t skybox_shader(
            "assets/shaders/skybox.vertex.shader",
            "assets/shaders/skybox.fragment.shader"
    );

    camera main_camera{};

    auto drag_callback = [&main_camera]() {
        auto const &delta = ImGui::GetMouseDragDelta();
        main_camera.rotate(glm::vec2(glm::radians(-delta.y), glm::radians(-delta.x)));
        ImGui::ResetMouseDragDelta();
    };

    auto scroll_callback = [&main_camera](ImGuiIO const &io, float mouse_delta) {
        float delta = 1 + mouse_delta * 0.2f;
        if (io.MouseWheel > 0) {
            main_camera.zoom_in(delta);
        } else {
            main_camera.zoom_out(delta);
        }
    };

    // Setup GUI context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    auto const start_time = std::chrono::steady_clock::now();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Get windows size
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // Set viewport to fill the whole window area
        glViewport(0, 0, display_w, display_h);

        // Fill background with solid color
        glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        glClearDepth(1);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Gui start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // GUI
        ImGui::Begin("Settings");
        ImGui::Text("Ambient light");
        static float ambient_color[4] = {0.5f, 0.5f, 0.5f, 1.0f};
        ImGui::ColorEdit3("color", ambient_color);

        ImGui::Separator();

        ImGui::Text("Material");
        static float reflectivity = 0.6f;
        ImGui::SliderFloat("reflectivity", &reflectivity, 0.0f, 1.0f);
        static bool enable_fresnel = true;
        ImGui::Checkbox("enable Fresnel", &enable_fresnel);
        static float shlick_coefficient = 5.0f;
        ImGui::SliderFloat("Shlick coefficient", &shlick_coefficient, 0.0f, 10.0f);
        static float refraction_coefficient = 0.5f;
        ImGui::SliderFloat("refraction coefficient", &refraction_coefficient, 0.0f, 1.0f);
        static float refractive_index = 1.0f;
        ImGui::SliderFloat("refractive index", &refractive_index, 0.5f, 5.0f);
        ImGui::End();

        float mouse_delta = std::abs(io.MouseWheel);
        if (!io.WantCaptureMouse && mouse_delta > 0.01f) {
            scroll_callback(io, mouse_delta);
        }

        if (!io.WantCaptureMouse && ImGui::IsMouseDragging()) {
            drag_callback();
        }

        auto const &camera_position = main_camera.position();
        auto model = glm::mat4(1) * glm::scale(glm::vec3(0.05, 0.05, 0.05));
        auto view = glm::lookAt<float>(camera_position, main_camera.target(), main_camera.up());
        auto skybox_view = glm::mat4(glm::mat3(view));

        auto projection = glm::perspective<float>(90, float(display_w) / display_h, 0.01, 100);

        // Bind triangle shader
        triangle_shader.use();

        triangle_shader.set_uniform(
                "u_ambient",
                ambient_color[0],
                ambient_color[1],
                ambient_color[2]
        );

        triangle_shader.set_uniform("u_camera_pos", camera_position.x, camera_position.y, camera_position.z);
        triangle_shader.set_uniform("u_model", glm::value_ptr(model));
        triangle_shader.set_uniform("u_view", glm::value_ptr(view));
        triangle_shader.set_uniform("u_projection", glm::value_ptr(projection));

        triangle_shader.set_uniform("u_tex", int(0));
        triangle_shader.set_uniform("u_skybox", int(1));

        triangle_shader.set_uniform("u_enable_fresnel", enable_fresnel);
        triangle_shader.set_uniform("u_reflectivity", reflectivity);
        triangle_shader.set_uniform("u_shlick_coefficient", shlick_coefficient);
        triangle_shader.set_uniform("u_refraction_coefficient", refraction_coefficient);
        triangle_shader.set_uniform("u_refractive_index", refractive_index);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

        for (const auto &object : objects_to_draw) {
            glActiveTexture(GL_TEXTURE0);

            std::string const &diffuse_texture = materials[object.material_id].diffuse_texname;
            if (textures.find(diffuse_texture) != textures.end()) {
                glBindTexture(GL_TEXTURE_2D, textures[diffuse_texture]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            } else {
                glBindTexture(GL_TEXTURE_2D, default_texture);
            }

            glBindVertexArray(object.vao);
            glDrawElements(GL_TRIANGLES, 3 * object.triangles_count, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);

        glDepthFunc(GL_LEQUAL);
        skybox_shader.use();

        skybox_shader.set_uniform("u_projection", glm::value_ptr(projection));
        skybox_shader.set_uniform("u_view", glm::value_ptr(skybox_view));
        skybox_shader.set_uniform("u_skybox", int(1));

        glBindVertexArray(skybox.vao);
        glDrawElements(GL_TRIANGLES, 3 * skybox.triangles_count, GL_UNSIGNED_INT, 0);

        glDepthFunc(GL_LESS);
        glBindVertexArray(0);

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

    glDeleteVertexArrays(1, &skybox.vao);
    glDeleteBuffers(1, &skybox.vbo);
    glDeleteBuffers(1, &skybox.ebo);

    for (const auto &item : objects_to_draw) {
        glDeleteVertexArrays(1, &item.vao);
        glDeleteBuffers(1, &item.vbo);
        glDeleteBuffers(1, &item.ebo);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
