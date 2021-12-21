#version 330

//material
struct Material{
    sampler2D mat_Diffuse; //Diffuse
    sampler2D mat_Specular; //Specular
    float mat_Shine; //shiny
};

//point light
struct PointLight{
    vec3 point_Position; //position

    vec3 point_Ambient; //ambient
    vec3 point_Diffuse; //diffuse
    vec3 point_Specular; //specular

    float point_constA; //constant
    float point_linA; //linear
    float point_expA; //quadratic / specular
};

out vec4 FragmentColor; //Color

in vec3 FragPos;

uniform sampler2D Texture;
in vec2 texCoord;

in vec3 normal;
in float hasNormals;

uniform float Transparency;
uniform vec3 viewPos;
uniform Material material;
uniform PointLight lit;

void main()
{
    //ambi
    vec3 ambience = vec3(lit.point_Ambient) * vec3(texture(material.mat_Diffuse, texCoord));
    //diff
    vec3 base = normalize(normal);
    vec3 lDir = normalize(lit.point_Position - FragPos);
    float div = max(dot(base, lDir), 0.0f);
    vec3 diff = vec3(lit.point_Diffuse) * div * vec3(texture(material.mat_Diffuse, texCoord));
    //quad
    vec3 vDir = normalize(viewPos - FragPos);
    vec3 rDir = reflect(-lDir, base);
    float quad = pow(max(dot(vDir, rDir), 0.0f), material.mat_Shine);
    vec3 spec = vec3(lit.point_Specular) * quad * vec3(texture(material.mat_Specular, texCoord));
    //attenuation - factor in distance, etc.
    float distance = length(lit.point_Position - FragPos);
    float atten = 1.0f / (lit.point_constA + lit.point_linA * distance + lit.point_expA * (distance*distance));
    //goes onto Ambient, Diffuse and Specular
    ambience *= atten;
    spec *= atten;
    diff *= atten;
    //result
    vec3 res = ambience + diff + spec;
    FragmentColor = vec4(res, 1.0f);
    FragmentColor.r = 1.0f;
    FragmentColor.g = 1.0f;
    FragmentColor.b = 0.0f;
    FragmentColor.a = Transparency;
}
