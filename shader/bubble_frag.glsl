#version 330
// A texture is expected as program attribute
uniform sampler2D Texture;

// Direction of light
uniform vec3 LightDirection;

// (optional) Transparency
uniform float Transparency;

// (optional) Texture offset
uniform vec2 TextureOffset;

// The vertex shader will feed this input
in vec2 texCoord;

// Wordspace normal passed from vertex shader
in vec4 normal;

// The final color
out vec4 FragmentColor;

float near = 0.1f;
float far = 150.0f;

float linearizeDepth(float depth){
  return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

void main() {
  float linear_depth = linearizeDepth(gl_FragCoord.z) / far;
  FragmentColor = texture(Texture, vec2(texCoord.x, 1.0 - texCoord.y) + TextureOffset);

  vec3 farColor = vec3(0.09, 0.05, 0.52);
  vec3 multiplyColor = vec3(0.529, 0.803, 1);

  FragmentColor.a = FragmentColor.r * Transparency;
  FragmentColor.r = mix(FragmentColor.r * multiplyColor.r, farColor.r, linear_depth);
  FragmentColor.g = mix(FragmentColor.g * multiplyColor.g, farColor.g, linear_depth);
  FragmentColor.b = mix(FragmentColor.b * multiplyColor.b, farColor.b, linear_depth);

}