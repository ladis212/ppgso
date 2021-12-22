#version 330

//material
struct Material{
    sampler2D mat_Diffuse; //Diffuse
    sampler2D mat_Specular; //Specular
    float mat_Shine; //shiny
};

//point light
struct PointLight{
    vec3 position; //position

    vec3 ambient; //ambient
    vec3 point_Diffuse; //diffuse
    vec3 point_Specular; //specular

    float constant; //constant
    float linear; //linear
    float specular; //quadratic / specular
};

out vec4 FragmentColor; //Color

in vec3 FragPos;

uniform sampler2D Texture;
in vec2 texCoord;

in vec4 normal;
in float hasNormals;

uniform float Transparency;
uniform vec3 viewPos;
uniform Material material;
uniform PointLight light;

void main()
{
    //ambi
    vec3 ambience = vec3(light.ambient) * vec3(texture(material.mat_Diffuse, texCoord));
    //diff
    vec3 base = normalize(vec3(normal));
    vec3 lDir = normalize(light.position - FragPos);
    float div = max(dot(base, lDir), 0.0f);
    vec3 diff = vec3(light.point_Diffuse) * div * vec3(texture(material.mat_Diffuse, texCoord));
    //quad
    vec3 vDir = normalize(viewPos - FragPos);
    vec3 rDir = reflect(-lDir, base);
    float quad = pow(max(dot(vDir, rDir), 0.0f), material.mat_Shine);
    vec3 spec = vec3(light.point_Specular) * quad * vec3(texture(material.mat_Specular, texCoord));
    //attenuation - factor in distance, etc.
    float distance = length(light.position - FragPos);
    float atten = 1.0f / (light.constant + light.linear * distance + light.specular * (distance*distance));
    //goes onto Ambient, Diffuse and Specular
    ambience *= atten;
    spec *= atten;
    diff *= atten;
    //result
    vec3 res = ambience + diff + spec;
    FragmentColor = vec4(res, 1.0f);
    //FragmentColor.r = 1.0f;
    //FragmentColor.g = 1.0f;
    //FragmentColor.b = 0.0f;
    FragmentColor.a = Transparency;
}
