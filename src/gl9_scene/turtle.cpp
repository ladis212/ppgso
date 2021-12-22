#include <glm/gtc/random.hpp>
#include "turtle.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <shaders/underwater_vert_glsl.h>
#include <shaders/underwater_frag_glsl.h>



// Static resources
std::unique_ptr<ppgso::Mesh> Turtle::mesh;
std::unique_ptr<ppgso::Texture> Turtle::texture;
std::unique_ptr<ppgso::Shader> Turtle::shader;

Turtle::Turtle() {
    // Set random scale speed and rotation
    //scale *= glm::linearRand(.1f, .3f);
    //speed = {glm::linearRand(-2.0f, 2.0f), glm::linearRand(-5.0f, -10.0f), 0.0f};
    rotation = {0, 0, 0};
    rotMomentum = {0, 0, 0};

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(underwater_vert_glsl, underwater_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("turtle\\turtle.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("turtle\\turtle.obj");
}

bool Turtle::update(Scene &scene, float dt) {
    // Count time alive
    //age += dt;

    // Animate position according to time
    position += speed * dt;
    rotation += rotMomentum * dt;


    generateModelMatrix();

    return true;
}

void Turtle::render(Scene &scene) {
    shader->use();
    shader->setUniform("LightDirection", scene.lightDirection);

    //glm::vec3 ambientLight(1, 0.5, 0.5);
    //shader->setUniform("AmbientLight", ambientLight);
    //float light_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f };
    //float mat_ambient[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    //glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    //glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);

    // use camera
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    shader->setUniform("Transparency", 1);


    mesh->render();
}

void Turtle::onClick(Scene &scene) {
    std::cout << "Turtle clicked!" << std::endl;
  //  age = 10000;
}

