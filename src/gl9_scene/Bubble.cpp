#include <glm/gtc/random.hpp>
#include "Bubble.h"

#include <shaders/texture_vert_glsl.h>
#include <shaders/bubble_frag_glsl.h>
#include <glm/gtx/euler_angles.hpp>


// Static resources
std::unique_ptr<ppgso::Mesh> Bubble::mesh;
std::unique_ptr<ppgso::Texture> Bubble::texture;
std::unique_ptr<ppgso::Shader> Bubble::shader;

Bubble::Bubble() {
    // Set random scale speed and rotation
    //scale *= glm::linearRand(.1f, .3f);
    //speed = {glm::linearRand(-2.0f, 2.0f), glm::linearRand(-5.0f, -10.0f), 0.0f};
    rotation = {0, 0, 0};
    rotMomentum = {0, 0, 0};

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(texture_vert_glsl, bubble_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("bubble.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("quad.obj");
}


bool Bubble::update(Scene &scene, float dt) {
    // Count time alive
    age += dt;

    // Animate position according to time
    position += speed * dt;
    rotation += rotMomentum * dt;

    // Delete when alive longer than 10s or out of visibility
    if (age > 30.0f || position.y > 20) return false;

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
    modelMatrix *= glm::inverse(scene.camera->viewMatrix);
    modelMatrix *= glm::orientate4(glm::vec3{0, 0, M_PI});

    return true;
}

void Bubble::render(Scene &scene) {
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

void Bubble::onClick(Scene &scene) {
    std::cout << "Bubble clicked!" << std::endl;
    age = 10000;
}

