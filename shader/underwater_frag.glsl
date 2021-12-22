#version 330

// A texture is expected as program attribute
uniform sampler2D Texture;

// Direction of light
uniform vec3 LightDirection;

// (optional) Transparency
uniform float Transparency;

// (optional) Texture offset
uniform vec2 TextureOffset;

uniform float HasNormals = 1.0f;
uniform int castShadows = 0;

// The vertex shader will feed this input
in vec2 texCoord;

// Wordspace normal passed from vertex shader
in vec4 normal;
in vec3 localNormal;
in vec4 worldPosition;
//in float Time;

// The final color
out vec4 FragmentColor;

struct PointLight{
  vec3 color; //rgb
  vec3 position; //position

  vec3 ambient; //ambient
  vec3 diffuse; //diffuse
  vec3 specular; //specular

  float constant; //constant
  float linear; //linear
  float quadratic; //quadratic / specular
};

PointLight lights[3];
uniform vec3 cameraView = vec3(0, 0, 0);

uniform vec3 Shadows[30];

float near = 0.1f;
float far = 150.0f;
float seaLevel = 0;//sin(Time)/3 + sin(float(worldPosition.x)/3 + Time)/2 + cos(float(worldPosition.z)/3 + Time)/2 + sin(float(worldPosition.x)/9 + Time)/2;

float linearizeDepth(float depth){
  return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float normalizeHeight(float y){
  return clamp(-y / 20, 0.0, 1.0);
}

vec3 getPointLightVector(PointLight light, vec3 fragmentPosition){
  vec4 fixed_normal = (HasNormals > 0.0) ? normal : vec4(0, 1, 0, 0);
  vec3 fixed_localNormal = (HasNormals > 0.0) ? localNormal : vec3(0, 1, 0);

  vec3 lightDir = normalize(light.position - fragmentPosition);
  float lightDiffuse = max(dot(fixed_normal, vec4(normalize(lightDir), 1.0)), 0.0);
  vec3 reflectionDir = reflect(-lightDir, fixed_localNormal);
  float lightSpecular = pow(max(dot(normalize(cameraView), reflectionDir), 0.0), 120);
  float distance = distance(light.position, fragmentPosition);
  float attenuation = 1.0 / (light.constant * light.linear * distance
                              + light.quadratic * pow(distance, 2));

  vec3 ambientColor = light.ambient * attenuation;
  vec3 diffuseColor = light.diffuse * lightDiffuse * attenuation;
  vec3 specularColor = light.specular * lightSpecular * attenuation;

  return (ambientColor + diffuseColor + specularColor) * light.color;
}

void main() {
  // Compute diffuse lighting
  float diffuse;


  if (HasNormals > 0) diffuse = max(dot(normal, vec4(normalize(LightDirection), 1.0f)), 0.0f);
  else diffuse = 1;

  // Lookup the color in Texture on coordinates given by texCoord
  // NOTE: Texture coordinate is inverted vertically for compatibility with OBJ
  float linear_depth = linearizeDepth(gl_FragCoord.z) / far;
  FragmentColor = texture(Texture, vec2(texCoord.x, 1.0 - texCoord.y) + TextureOffset) * diffuse;
  if(worldPosition.y < seaLevel){
    vec3 farColor = vec3(0.09, 0.05, 0.52);
    vec3 multiplyColor = vec3(0.529, 0.803, 1);

    FragmentColor.r = mix(FragmentColor.r * multiplyColor.r, farColor.r, linear_depth);
    FragmentColor.g = mix(FragmentColor.g * multiplyColor.g, farColor.g, linear_depth);
    FragmentColor.b = mix(FragmentColor.b * multiplyColor.b, farColor.b, linear_depth);
  }

  if(castShadows == 1){ //vrhanie tienov rybkami
    float yNorm;
    for(int i = 0; i < 30; i++){
      yNorm = normalizeHeight(Shadows[i].y);
      if(distance(vec2(worldPosition.x, worldPosition.z), vec2(Shadows[i].x, Shadows[i].z)) < 2.0 - yNorm){
        FragmentColor.r *= 1 - (yNorm) * 0.5;
        FragmentColor.g *= 1 - (yNorm) * 0.5;
        FragmentColor.b *= 1 - (yNorm) * 0.5;

      }
    }
  }

  for(int i = 0; i < 3; i++){
    vec3 colorVectors[] = {vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1)};
    lights[i] = PointLight(colorVectors[i], vec3(-15*i, -15, 0), vec3(0.2, 0.2, 0.2), vec3(0.5, 0.5, 0.5), vec3(0.1, 0.1, 0.1), 1.0, 0.09, 0.032);
    FragmentColor = FragmentColor + vec4(getPointLightVector(lights[i], vec3(worldPosition)), Transparency);
  }



}
