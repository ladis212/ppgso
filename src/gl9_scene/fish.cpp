//hmm tato genericka funkcia sa mi da pouzit...

#include <glm/gtc/random.hpp>
#include "fish.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>




// Static resources
std::unique_ptr<ppgso::Mesh> fish::mesh;
std::unique_ptr<ppgso::Texture> fish::texture;
std::unique_ptr<ppgso::Shader> fish::shader;

fish::fish(const std::basic_string<char> &mesh_path, const std::basic_string<char> &texture_path, const std::string &vert, const std::string &frag) {
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>(mesh_path);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP(texture_path));
    if (!shader) shader = std::make_unique<ppgso::Shader>(vert, frag);
    // Set random scale speed and rotation
    //scale *= glm::linearRand(.1f, .3f);
    //speed = {glm::linearRand(-2.0f, 2.0f), glm::linearRand(-5.0f, -10.0f), 0.0f};
    rotation = {0, 0, 0};
    rotMomentum = {0, 0, 0};

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
}

bool fish::update(Scene &scene, float dt) {
    // Count time alive
    //age += dt;

    // Animate position according to time
    position += speed * dt;
    rotation += rotMomentum * dt;

    generateModelMatrix();

    return true;
}

void fish::render(Scene &scene) {
    shader->use();
    shader->setUniform("LightDirection", scene.lightDirection);

    // use camera
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    shader->setUniform("Transparency", 1);
    mesh->render();
}

void fish::onClick(Scene &scene) {
    std::cout << "Dolphin clicked!" << std::endl;
    age = 10000;
}

