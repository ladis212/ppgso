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

out vec4 point_Color; //Color

in vec3 FragPos;
in vec3 Normal;
in vec3 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform PointLight lit;

/*vec3 PLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.point_Position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.point_Position - fragPos);
    float attenuation = 1.0 / (light.point_constA + light.point_linA * distance +
    light.point_expA * (distance * distance));
    // combine results
    vec3 ambient  = light.point_Ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.point_Diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.point_Specular * spec * vec3(texture(material.specular, TexCoords));
    point_Ambient  *= attenuation;
    point_Diffuse  *= attenuation;
    point_Specular *= attenuation;
    return (point_Ambient +  point_Diffuse + point_Specular);
}*/

void main()
{
    //ambi
    vec3 ambience = lit.point_Ambient * vec3(texture(material.mat_Diffuse, TexCoords));
    //diff
    vec3 base = normalize(Normal);
    vec3 lDir = normalize(lit.point_Position - FragPos);
    float div = max(dot(base, lDir), 0.0f);
    vec3 diff = lit.point_Diffuse * diff * vec3(texture(material.mat_Diffuse, TexCoords));
    //quad
    vec3 vDir = normalize(viewPos - FragPos);
    vec3 rDir = reflect(-lDir, base);
    float quad = pow(max(dot(vDir, rDir), 0.0f), material.mat_Shine);
    vec3 spec = lit.point_Specular * quad * vec3(texture(material.mat_Specular, TexCoords));
    //attenuation - factor in distance, etc.
    float distance = length(lit.point_Position - FragPos);
    float atten = 1.0f / (lit.point_constA + lit.point_linA * distance + lit.point_expA * (distance*distance));
    //goes onto Ambient, Diffuse and Specular
    ambience *= atten;
    spec *= atten;
    diff *= atten;
    //result
    vec3 res = ambience + diff + spec;
    point_Color = vec4(result, 1.0f);
}
