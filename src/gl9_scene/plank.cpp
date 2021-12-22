#include <glm/gtc/random.hpp>
#include "plank.h"
#include "island.h"
#include "dolphin.h"
#include "GenericObject.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <shaders/underwater_vert_glsl.h>
#include <shaders/underwater_frag_glsl.h>




// Static resources
std::unique_ptr<ppgso::Mesh> Plank::mesh;
std::unique_ptr<ppgso::Texture> Plank::texture;
std::unique_ptr<ppgso::Shader> Plank::shader;

Plank::Plank() {

    //rotation and rotmomentum
    rotation = {0, 0, 0};
    rotMomentum = {0, 0, 0};

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("plank\\grunge-wood-texture.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("plank\\wornout_plank.obj");
}

bool Plank::update(Scene &scene, float dt) {
    // Animate position according to time
    position += speed * dt;

    // Rotate the object
    rotation += rotMomentum * dt;

    // Collide with scene - this plank is very fragile.
    for (auto &obj : scene.objects) {
        // Ignore self in scene
        if (obj.get() == this) continue;

        //Possible collides:
        auto island = dynamic_cast<Island*>(obj.get()); // collision With Island
        auto dolphin = dynamic_cast<Dolphin*>(obj.get()); //collision With Dolphin
        auto plunk = dynamic_cast<Plank*>(obj.get()); //Collision With Another Plank
        auto shark = dynamic_cast<GenericObject*>(obj.get()); //Collision with Shark (no other generic object can do that...)
        if (!island && !dolphin && !plunk && !shark) continue;

        // hitbox
        if (distance(position, obj->position) < (obj->scale.y + scale.y) * 0.7f) {
            int pieces = glm::linearRand(2, 3); //2 or 3 pieces

            // if board is tiny it just gets destroy.
            if (scale.x < 0.25) pieces = 0;


            // Generate smaller asteroids
            shatter(scene, pieces);
            // Destroy self
            return false;
        }
    }

    // Generate modelMatrix from position, rotation and scale
    generateModelMatrix();

    return true;
}

void Plank::shatter(Scene &scene, int pieces){
    // Generate smaller planks
        for (int i = 0; i < pieces; i++) {
            auto plank = std::make_unique<Plank>();
            plank->speed = speed + glm::vec3(glm::linearRand(-3.0f, 3.0f), glm::linearRand(0.0f, -5.0f), 0.0f);;
            plank->position = position;
            plank->rotMomentum = rotMomentum;
            float factor = (float) pieces / 2.0f;
            plank->scale = scale / factor;
            scene.objects.push_back(move(plank));
        }
}

void Plank::render(Scene &scene) {
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

void Plank::onClick(Scene &scene) {
    std::cout << "Island clicked!" << std::endl;
    //age = 10000;
}

