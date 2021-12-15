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
float far = 20.0f;

float linearizeDepth(float depth){
  return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

void main() {
  // Compute diffuse lighting
  float diffuse = max(dot(normal, vec4(normalize(LightDirection), 1.0f)), 0.0f);

  // Lookup the color in Texture on coordinates given by texCoord
  // NOTE: Texture coordinate is inverted vertically for compatibility with OBJ
  float linear_depth = linearizeDepth(gl_FragCoord.z) / far;
  FragmentColor = texture(Texture, vec2(texCoord.x, 1.0 - texCoord.y) + TextureOffset) * diffuse;
  vec3 multiplyColor = {
  0.09,0.05,0.52
  };

  FragmentColor.r = mix(FragmentColor.r, FragmentColor.r * multiplyColor.r, linear_depth);
  FragmentColor.g = mix(FragmentColor.g, FragmentColor.g * multiplyColor.g, linear_depth);
  FragmentColor.b = mix(FragmentColor.b, FragmentColor.b * multiplyColor.b, linear_depth);

  FragmentColor.a = Transparency;
}