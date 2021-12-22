#include <glm/gtc/random.hpp>
#include "stingray.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <shaders/underwater_vert_glsl.h>
#include <shaders/underwater_frag_glsl.h>



// Static resources
std::unique_ptr<ppgso::Mesh> Stingray::mesh;
std::unique_ptr<ppgso::Texture> Stingray::texture;
std::unique_ptr<ppgso::Shader> Stingray::shader;

Stingray::Stingray() {
    // Set random scale speed and rotation
    //scale *= glm::linearRand(.1f, .3f);
    //speed = {glm::linearRand(-2.0f, 2.0f), glm::linearRand(-5.0f, -10.0f), 0.0f};
    rotation = {0, 0, 0};
    rotMomentum = {0, 0, 0};

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(underwater_vert_glsl, underwater_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("stingray\\STING.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("stingray\\STINGRAY.obj");
}

bool Stingray::update(Scene &scene, float dt) {
    position += speed * dt;
    rotation += rotMomentum * dt;

    appear(scene, dt);
    generateModelMatrix();

    return true;
}

void Stingray::appear(Scene &scene, float dt){
    //Position
    clock_t t;
    t = clock();

    float v = 2 * sin(t/1000.0f);
    if (v > 1) v = 1;
    else if (v < -1) {
        v = -1;
    }
    position.y +=  v / 300.0f;

}

void Stingray::render(Scene &scene) {
    clock_t t;
    t = clock();

    shader->use();
    shader->setUniform("LightDirection", scene.lightDirection);

    // use camera
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    float v = 2 * sin(t/1000.0f);
    if (v < 0){
        if (v < -1) v = -1;
        shader->setUniform("Transparency", 1+v);
    }
    shader->setUniform("HasNormals", -1.0f);
    mesh->render();
}

void Stingray::onClick(Scene &scene) {
    std::cout << "Stingray clicked!" << std::endl;
    age = 10000;
}

