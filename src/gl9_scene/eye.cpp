#include <glm/gtc/random.hpp>
#include "eye.h"

#include <shaders/bubble_vert_glsl.h>
#include <shaders/bubble_frag_glsl.h>
#include <glm/gtx/euler_angles.hpp>

#include <glm/gtx/string_cast.hpp>


// Static resources

Eye::Eye(const std::basic_string<char> &texture_path, const std::string &vert, const std::string &frag) {
    // Set random scale speed and rotation
    //scale *= glm::linearRand(.1f, .3f);
    //speed = {glm::linearRand(-2.0f, 2.0f), glm::linearRand(-5.0f, -10.0f), 0.0f};
    rotation = {0, 0, 0};
    rotMomentum = {0, 0, 0};

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(vert, frag);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP(texture_path));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("quad.obj");
}


bool Eye::update(Scene &scene, float dt) {
    // Count time alive
    age += dt;

    // Animate position according to time
    position += speed * dt;
    rotation += rotMomentum * dt;

    // Delete when alive longer than 10s or out of visibility
    if (age > 30.0f || position.y > -2) return false;

    // Collide with scene
    //for (auto &obj : scene.objects) {
    // Ignore self in scene
    //if (obj.get() == this) continue;

    // We only need to collide with asteroids and projectiles, ignore other objects
    //auto asteroid = dynamic_cast<Island*>(obj.get()); // dynamic_pointer_cast<Island>(obj);
    //auto projectile = dynamic_cast<Projectile*>(obj.get()); //dynamic_pointer_cast<Projectile>(obj);
    //if (!asteroid && !projectile) continue;

    // When colliding with other asteroids make sure the object is older than .5s
    // This prevents excessive collisions when asteroids explode.
    //if (asteroid && age < 0.5f) continue;

    // Compare distance to approximate size of the asteroid estimated from scale.
    //if (distance(position, obj->position) < (obj->scale.y + scale.y) * 0.7f) {
    //  int pieces = 3;
//
    //  // Too small to split into pieces
    //  if (scale.y < 0.5) pieces = 0;
//
    //  // The projectile will be destroyed
    //  if (projectile) projectile->destroy();
//
    //  // Generate smaller asteroids
    //  explode(scene, (obj->position + position) / 2.0f, (obj->scale + scale) / 2.0f, pieces);
//
    // Destroy self
    //return false;
    //}
    //}

    // Generate modelMatrix from position, rotation and scale
    generateModelMatrix();
    //wqmodelMatrix *= glm::inverse(scene.camera->viewMatrix);
    //modelMatrix *= glm::orientate4(glm::vec3{0, 0, M_PI});

    return true;
}

void Eye::render(Scene &scene) {
    shader->use();
    shader->setUniform("LightDirection", scene.lightDirection);

    //glm::vec3 ambientLight(1, 0.5, 0.5);
    //shader->setUniform("AmbientLight", ambientLight);
    //float light_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f };
    //float mat_ambient[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    //glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    //glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);

    // use camera
    //glm::mat4 spriteMatrix = glm::mat4(1.0);
//
    //float d = sqrt( pow(scene.camera->viewMatrix[0][0], 2.0) + pow(scene.camera->viewMatrix[0][1], 2.0) + pow(scene.camera->viewMatrix[0][2], 2.0) );
//
    //for(int translate_axis = 0; translate_axis < 3; translate_axis++){
    //    spriteMatrix[3][translate_axis] = scene.camera->viewMatrix[3][translate_axis];
    //    spriteMatrix[translate_axis][translate_axis] = 5;
    //}
//
    //std::cout << glm::to_string(spriteMatrix);


    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    shader->setUniform("BubblePos", position);
    shader->setUniform("SingleScale", scale.x);

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    shader->setUniform("Transparency", 1);

    shader->setUniform("Time", 1);
    mesh->render();
}

void Eye::onClick(Scene &scene) {
    std::cout << "Bubble clicked!" << std::endl;
    age = 10000;
}

