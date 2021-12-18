#include <glm/gtc/random.hpp>
#include "sponge.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <shaders/underwater_vert_glsl.h>
#include <shaders/underwater_frag_glsl.h>



// Static resources
std::unique_ptr<ppgso::Mesh> Sponge::mesh;
std::unique_ptr<ppgso::Texture> Sponge::texture;
std::unique_ptr<ppgso::Shader> Sponge::shader;

Sponge::Sponge() {
    // Set random scale speed and rotation
    rotation = {0, 0, 0};
    rotMomentum = {0, 0, 0};

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(underwater_vert_glsl, underwater_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("sponge\\sponge.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sponge\\sponge.obj");
}

bool Sponge::update(Scene &scene, float dt) {

    // Animate position according to time
    position += speed * dt;
    rotation += rotMomentum * dt;
    breathe(scene);
    generateModelMatrix();

    return true;
}

void Sponge::breathe(Scene &scene){
    clock_t t;
    t = clock();
    rotation.x = -M_PI_2;
    scale.x += sin(t/1000.0f) / 6000.0f ;
    scale.z += sin(t/1000.0f) / 6000.0f;
}

void Sponge::render(Scene &scene) {
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

void Sponge::onClick(Scene &scene) {
    std::cout << "Sponge clicked!" << std::endl;
    age = 10000;
}

