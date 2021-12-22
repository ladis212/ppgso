#version 330
// The inputs will be fed by the vertex buffer objects
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec3 Normal;

// Matrices as program attributes
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform float Time;

// This will be passed to the fragment shader
out vec2 texCoord;
out vec4 normal;
out vec3 eyePosition;

void main() {
  // Copy the input to the fragment shader
  texCoord = TexCoord;
  //float real_x = Position.x;
  //float real_z = Position.z;
  //float result = sin(Time)/3 + sin(float(real_x)/3 + Time)/2 + cos(float(real_z)/3 + Time)/2 + sin(float(real_x)/9 + Time)/2;
  //float x_tangent = sin(Time)/3 + cos(real_x/3 + Time)/2 + cos(float(real_z)/3 + Time)/2 + cos(float(real_x)/9 + Time)/2;
  //float z_tangent = sin(Time)/3 + sin(float(real_x)/3 + Time)/2 -sin(float(real_z)/3 + Time)/2 + sin(float(real_x)/9 + Time)/2;
  //vec3 xtan_vec = vec3(x_tangent, result, z_tangent);
  //vec3 ztan_vec = vec3(0, x_tangent, z_tangent);
  //normal = vec4(normalize(xtan_vec), 0);


  // Calculate the final position on screen
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Position, 1.0);
}
