#include <glm/gtc/random.hpp>
#include "sponge.h"
#include "bubble.h"

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
    breathe(scene, dt);
    generateModelMatrix();

    return true;
}

void Sponge::breathe(Scene &scene, float dt){
    clock_t t;
    t = clock();
    rotation.x = -M_PI_2;
    scale.x = 0.1 + 0.05*sin(t/1000.0f);
    scale.y = 0.1 + 0.05*sin(t/1000.0f);
    scale.z = 0.1 + 0.05*sin(t/1000.0f);

    // Accumulate time
    time += dt;

    //Add object to scene when time reaches certain level
    if (time > 0.2 * (1 - sin(t/1000.0f + 3))) {
        auto obj = std::make_unique<Bubble>();
        obj->position = position;
        obj->rotation = {0, 0, M_PI};
        obj->position.x = glm::linearRand(position.x, position.x+0.5f);
        obj->position.z = glm::linearRand(position.z, position.z+0.5f);
        float randScale = glm::linearRand(0.05f, 0.2f);
        obj->scale = glm::vec3(randScale);
        float randSpeed = glm::linearRand(4.0f, 5.0f);
        obj->speed = {0, randSpeed, 0};
        scene.objects.push_back(move(obj));
        time = 0;
    }


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

