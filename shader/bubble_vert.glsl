#version 330
// The inputs will be fed by the vertex buffer objects
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec3 Normal;

// Matrices as program attributes
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec3 BubblePos;
uniform float SingleScale;

uniform float Time;

// This will be passed to the fragment shader
out vec2 texCoord;

// Normal to pass to the fragment shader
out vec4 normal;

out float x;
out float y;
out float time;

void main() {
  // Copy the input to the fragment shader
  texCoord = TexCoord;
  x = Position.x;
  y = Position.y;
  time = Time;

  // Normal in world coordinates
  normal = normalize(ModelMatrix * vec4(Normal, 0.0f));

  //spriteMatrix[3][1] = - spriteMatrix[3][1];
  vec3 CameraRight_worldspace = {ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]};
  vec3 CameraUp_worldspace = {ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]};


  vec3 Pos = vec3(gl_Position.x,gl_Position.y,gl_Position.z) + BubblePos;
  // Calculate the final position on screen
  gl_Position = (ProjectionMatrix * ViewMatrix) * vec4(Pos + CameraRight_worldspace * Position.x * SingleScale + CameraUp_worldspace * Position.y * SingleScale, 1.0);
  //gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix


}