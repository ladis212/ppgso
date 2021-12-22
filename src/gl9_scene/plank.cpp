#include <glm/gtc/random.hpp>
#include "plank.h"
#include "island.h"
#include "dolphin.h"
#include "GenericObject.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <shaders/underwater_vert_glsl.h>
#include <shaders/underwater_frag_glsl.h>

#define PHASEDIFF 1000
#define WIND_MAX 5.0f
#define THRESHHOLD 20

// Static resources
std::unique_ptr<ppgso::Mesh> Plank::mesh;
std::unique_ptr<ppgso::Texture> Plank::texture;
std::unique_ptr<ppgso::Shader> Plank::shader;
glm::vec3 wind = {0.0f, 0.0f, 0.0f};

Plank::Plank() {
  //  std::cout << "go" << std::endl;
    //rotation and rotmomentum
    rotation = {0, 0, 0};
    momentum = {0, 0, 0};

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("plank\\grunge-wood-texture.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("plank\\wornout_plank.obj");
}

bool Plank::update(Scene &scene, float dt) {
    // Animate position according to time
    if (phaser == 0) wind = {glm::linearRand(-WIND_MAX, WIND_MAX), 0, glm::linearRand(-WIND_MAX, WIND_MAX)};
    position += (wind + momentum)* dt ; //Momentum 0 on first plank, adds up if collided.

    //increment phaser
    phaser++;
    if (momentum.x * wind.x < 0) { //The momentum is against the wind
        momentum.x -= fmin(abs(wind.x + 0.5f), momentum.x);
    }
    else { //Momentum is in the direction of the wind.
        momentum.x -= fmin(1, momentum.x);
    }
    //same for momentum z
    if (momentum.z * wind.z < 0) { //The momentum is against the wind
        momentum.z -= fmin(abs(wind.z + 0.5f), momentum.z);
    }
    else { //Momentum is in the direction of the wind.
        momentum.z -= fmin(1, momentum.z);
    }

    if (phaser >= PHASEDIFF) phaser = 0; //ready for wind change


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
        if ((phaser-immunity >= THRESHHOLD)&&(distance(position, obj->position) < (obj->scale.x + scale.x) * 0.7f)) {
            std::cout << "We collide." << std::endl;
            int pieces = glm::linearRand(2, 3); //2 or 3 pieces

            // if board is tiny it just gets destroy.
            if (scale.x <= 0.33f) pieces = 0;


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
    std::cout << "We shatter" << std::endl;
        for (int i = 0; i < pieces; i++) {
            auto plank = std::make_unique<Plank>();
            plank->momentum =glm::vec3(glm::linearRand(-20.0f, 20.0f), 0.0f , glm::linearRand(-20.0f, 20.0f));;
            plank->position = position;
            //plank->rotMomentum = rotMomentum;
            plank->scale = {scale.x/pieces, scale.y/pieces, scale.z/pieces};
            std::cout << "We're about to shatter!" << std::endl;
            scene.objects.push_back(move(plank));
            std::cout << "shattered" << std::endl;
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

