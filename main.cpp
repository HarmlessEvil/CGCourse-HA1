#include <iostream>
#include <vector>
#include <chrono>

#include <fmt/format.h>

#include <GL/glew.h>

// Imgui + bindings
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Math constant and routines for OpenGL interop
#include <glm/gtc/constants.hpp>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/vec2.hpp>

#include "opengl_shader.h"

static void glfw_error_callback(int error, const char *description)
{
   std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

unsigned int load_colormap()
{
   int width, height, channel_amount;
   unsigned char *data = stbi_load(
           "palettes/spectral_colormap.png",
           &width,
           &height,
           &channel_amount,
           0
   );

   if (!data) {
      throw std::runtime_error("Failed to load texture");
   }

   unsigned int texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_1D, texture);

   glTexImage1D(
           GL_TEXTURE_1D,
           0,
           channel_amount == 3 ? GL_RGB : GL_RGBA,
           width,
           0,
           channel_amount == 3 ? GL_RGB : GL_RGBA,
           GL_UNSIGNED_BYTE, data
   );
   glGenerateMipmap(GL_TEXTURE_1D);

   stbi_image_free(data);

   return texture;
}

void create_triangle(GLuint &vbo, GLuint &vao, GLuint &ebo)
{
   // create the triangle
   float triangle_vertices[] = {
       -1.0f, -1.0f, 0.0f,	 // position vertex 1
        1.0f, -1.0f, 0.0f,  // position vertex 2
       -1.0f,  1.0f, 0.0f,  // position vertex 3
        1.0f,  1.0f, 0.0f,  // position vertex 4
   };
   unsigned int triangle_indices[] = {
       0, 1, 2, 2, 3, 1 };
   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &ebo);
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

struct mouse_state {
    bool is_dragging = false;
    ImVec2 last_drag_delta = {0, 0};
};

int main(int, char **)
{
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
   GLFWwindow *window = glfwCreateWindow(1280, 1280, "Dear ImGui - Conan", NULL, NULL);
   if (window == NULL)
      return 1;
   glfwMakeContextCurrent(window);
   glfwSwapInterval(1); // Enable vsync

   // Initialize GLEW, i.e. fill all possible function pointers for current OpenGL context
   if (glewInit() != GLEW_OK)
   {
      std::cerr << "Failed to initialize OpenGL loader!\n";
      return 1;
   }

   // create our geometries
   GLuint vbo, vao, ebo;
   create_triangle(vbo, vao, ebo);
   unsigned int colormap = load_colormap();

   // init shader
   shader_t triangle_shader("simple-shader.vs", "simple-shader.fs");

   // Setup GUI context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init(glsl_version);
   ImGui::StyleColorsDark();

   mouse_state mouse_state{};

   glm::vec2 shift = {0, 0};
   float scale = 1;

   auto const start_time = std::chrono::steady_clock::now();

   while (!glfwWindowShouldClose(window))
   {
      glfwPollEvents();

      // Get windows size
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);

      // Set viewport to fill the whole window area
      glViewport(0, 0, display_w, display_h);

      // Fill background with solid color
      glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
      glClear(GL_COLOR_BUFFER_BIT);

      // Gui start new frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // GUI
      ImGui::Begin("Settings");
      ImGui::Text("Fractal options");
      static float max_radius = 130.0;
      ImGui::SliderFloat("radius", &max_radius, 2, 1000);
      static int max_iterations = 64;
      ImGui::SliderInt("iterations", &max_iterations, 10, 400);

      ImGui::Separator();

      ImGui::Text("Other settings");
      static float scale_sensitivity = 0.25f;
      ImGui::SliderFloat("scale sensitivity", &scale_sensitivity, 0.01f, 0.5f);

      ImGui::End();

       float mouse_delta = std::abs(io.MouseWheel);
       if (!io.WantCaptureMouse && mouse_delta > 0.01) {
           glm::vec2 world_position_before {
               (io.MousePos.x / io.DisplaySize.x * 2 - 1) * scale,
               (io.MousePos.y / io.DisplaySize.y * 2 - 1) * scale
           };

           if (io.MouseWheel > 0) {
               scale /= 1 + mouse_delta * scale_sensitivity;
           } else {
               scale *= 1 + mouse_delta * scale_sensitivity;
           }

           glm::vec2 world_position_after {
                   (io.MousePos.x / io.DisplaySize.x * 2 - 1) * scale,
                   (io.MousePos.y / io.DisplaySize.y * 2 - 1) * scale
           };

           auto &&delta = world_position_after - world_position_before;

           shift.x -= delta.x;
           shift.y += delta.y;
       }

      if (!io.WantCaptureMouse && ImGui::IsMouseDragging()) {
         auto delta = ImGui::GetMouseDragDelta();

         delta.x = delta.x / io.DisplaySize.x * 2 * scale;
         delta.y = delta.y / io.DisplaySize.y * 2 * scale;

         shift.x -= delta.x;
         shift.y += delta.y;

         ImGui::ResetMouseDragDelta();
      }

      // Pass the parameters to the shader as uniforms
      triangle_shader.set_uniform("u_max_radius", max_radius);
      triangle_shader.set_uniform("u_max_iterations", max_iterations);
      triangle_shader.set_uniform("u_shift", shift.x, shift.y);
      triangle_shader.set_uniform("u_scale", scale);

      // Bind triangle shader
      triangle_shader.use();
      // Bind vertex array = buffers + indices
      glBindTexture(GL_TEXTURE_1D, colormap);
      glBindVertexArray(vao);
      // Execute draw call
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
