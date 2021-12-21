#include <glm/gtc/random.hpp>
#include "cave.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <shaders/underwater_vert_glsl.h>
#include <shaders/underwater_frag_glsl.h>



// Static resources
std::unique_ptr<ppgso::Mesh> Cave::mesh;
std::unique_ptr<ppgso::Texture> Cave::texture;
std::unique_ptr<ppgso::Shader> Cave::shader;

Cave::Cave() {
    rotation = {0, 0, 0};
    rotMomentum = {0, 0, 0};

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(underwater_vert_glsl, underwater_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("cave\\brownrock.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("cave\\cave.obj");
}

bool Cave::update(Scene &scene, float dt) {

    position += speed * dt;
    rotation += rotMomentum * dt;

    generateModelMatrix();

    return true;
}

void Cave::render(Scene &scene) {
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

void Cave::onClick(Scene &scene) {
    std::cout << "Cave clicked!" << std::endl;
    age = 10000;
}

