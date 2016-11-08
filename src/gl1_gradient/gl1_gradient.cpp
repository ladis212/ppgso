// Example gl1_gradient
// - Demonstrates the use of a dynamically generated gradient in GLSL
// - Displays the gradient on a quad using OpenGL

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <ppgso/ppgso.h>

#include "gl1_gradient_frag.h"
#include "gl1_gradient_vert.h"

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

class GradientWindow : public Window {
private:
  // Vertices to define polygon shape
  vector<vec2> vertex_buffer = {
      // x, y
      { 1.0f, 1.0f },
      { -1.0f, 1.0f },
      { 1.0f, -1.0f },
      { -1.0f, -1.0f }
  };

  // Colors for vertices
  vector<vec3> color_buffer = {
      // r, g, b
      { 1.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },
      { 0.0, 0.0f, 1.0f },
      { 1.0f, 1.0f, 1.0f }
  };

  Shader program = {gl1_gradient_vert, gl1_gradient_frag};
  GLuint vao, vbo, cbo;
public:
  GradientWindow() : Window{"gl1_gradient", SIZE, SIZE} {
    // Generate a vertex array object
    // This keeps track of our data buffers
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate a vertex buffer object, this will feed data to the vertex shader
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(vec2), vertex_buffer.data(), GL_STATIC_DRAW);

    // Setup vertex array lookup, this tells the shader how to pick data for the "Position" input
    auto position_attrib = program.GetAttribLocation("Position");
    glVertexAttribPointer(position_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_attrib);

    // Same thing for colors
    glGenBuffers(1, &cbo);
    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, color_buffer.size() * sizeof(vec3), color_buffer.data(), GL_STATIC_DRAW);

    auto color_attrib = program.GetAttribLocation("Color");
    glVertexAttribPointer(color_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(color_attrib);
  }

  ~GradientWindow() override {
    glDeleteBuffers(1, &cbo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  void onIdle() override {
    // Set gray background
    glClearColor(.5, .5, .5, 0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw triangles using the program
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }
};

int main()
{
  // Create a window with OpenGL 3.3 enabled
  auto window = GradientWindow{};

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
