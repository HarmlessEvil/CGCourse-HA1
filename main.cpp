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
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

struct drawable {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    std::size_t material_id;

    std::size_t triangles_count;
};

// Loads model from .obj file. Assumes that if the model uses some material defined in .mtl file, the material is stored
// in exact same directory as the .obj file itself.
void load_model(
        std::vector<drawable> &object_to_draw,
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
            auto ambient =  materials[material_id].ambient;

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

        object_to_draw.push_back(object);
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

    std::vector<drawable> objects_to_draw{};
    std::vector<tinyobj::material_t> materials{};

    std::unordered_map<std::string, GLuint> textures{};
    GLuint default_texture = generate_default_texture();

    load_model(objects_to_draw, materials, textures, std::filesystem::path("assets/models/cube/cube.obj"));

    // init shader
    shader_t triangle_shader(
            "assets/shaders/simple-shader.vs",
            "assets/shaders/simple-shader.fs"
    );

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

        // Set viewport to fill the whole window area
        glViewport(0, 0, display_w, display_h);

        // Fill background with solid color
        glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_CULL_FACE);

        // Gui start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // GUI
        ImGui::Begin("Triangle Position/Color");
        static float rotation = 0.0;
        ImGui::SliderFloat("rotation", &rotation, 0, 2 * glm::pi<float>());
        static float translation[] = {0.0, 0.0};
        ImGui::SliderFloat2("position", translation, -1.0, 1.0);

        ImGui::Separator();

        ImGui::Text("Ambient light");
        static float ambient_color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        ImGui::ColorEdit3("color", ambient_color);
        ImGui::End();

        // Pass the parameters to the shader as uniforms
        triangle_shader.set_uniform("u_rotation", rotation);
        triangle_shader.set_uniform("u_translation", translation[0], translation[1]);
        float const time_from_start = (float) (
                std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start_time).count() /
                1000.0);
        triangle_shader.set_uniform("u_time", time_from_start);
        triangle_shader.set_uniform(
                "u_ambient",
                ambient_color[0],
                ambient_color[1],
                ambient_color[2]
        );


        auto model = glm::rotate(glm::mat4(1), glm::radians(rotation * 60), glm::vec3(0, 1, 0)) *
                     glm::scale(glm::vec3(2, 2, 2));
        auto view = glm::lookAt<float>(glm::vec3(0, 0, -3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        auto projection = glm::perspective<float>(90, float(display_w) / display_h, 0.1, 100);
        auto mvp = projection * view * model;
        //glm::mat4 identity(1.0);
        //mvp = identity;
        triangle_shader.set_uniform("u_mvp", glm::value_ptr(mvp));
        triangle_shader.set_uniform("u_tex", int(0));


        // Bind triangle shader
        triangle_shader.use();

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
        glBindTexture(GL_TEXTURE_2D, 0);
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

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
