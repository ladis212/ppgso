#include "FishTail.h"
#include <cmath>
#include <glm/gtc/random.hpp>
#include <ppgso/ppgso.h>
#include <shaders/underwater_vert_glsl.h>
#include <shaders/underwater_frag_glsl.h>

// shared resources
std::unique_ptr<ppgso::Mesh> FishTail::mesh;
std::unique_ptr<ppgso::Texture> FishTail::texture;
std::unique_ptr<ppgso::Shader> FishTail::shader;



FishTail::FishTail() {
    if (!shader) shader = std::make_unique<ppgso::Shader>(underwater_vert_glsl, underwater_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("smallFish\\smallFish_texture.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("smallFish\\smallFish_tail.obj");

}



bool FishTail::update(Scene &scene, float time) {
    position.y = -1;
    rotation.y = (abs(std::fmod(time, 2.0f) - 1.0f)*2 - 1.0f) * glm::radians(30.0f);
    generateModelMatrix();
    return true;
}


void FishTail::render(Scene &scene) {
    shader->use();
    shader->setUniform("LightDirection", scene.lightDirection);

    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    shader->setUniform("Transparency", 1);
    shader->setUniform("HasNormals", 1.0f);
    mesh->render();


}


