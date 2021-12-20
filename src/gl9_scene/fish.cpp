//hmm tato genericka funkcia sa mi da pouzit...

#include <glm/gtc/random.hpp>
#include "fish.h"
#include "Bubble.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

//Boundary when fish returns...
#define BOUNDARY 20.0f

// Static resources

int funny = 0;
//int varination = glm::linearRand(0, 3);

fish::fish(const std::basic_string<char> &mesh_path, const std::basic_string<char> &texture_path, const std::string &vert, const std::string &frag) {

    mesh = std::make_unique<ppgso::Mesh>(mesh_path);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP(texture_path));
    if (!shader) shader = std::make_unique<ppgso::Shader>(vert, frag);
    // Set random scale speed and rotation
    //scale *= glm::linearRand(.1f, .3f);
    //speed = {glm::linearRand(-2.0f, 2.0f), glm::linearRand(-5.0f, -10.0f), 0.0f};
    rotation = {0, 0, 0};
    rotMomentum = {0, 0, 0};

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
}

bool fish::update(Scene &scene, float dt) {
    // Count time alive
    //age += dt;

    // Animate position according to time
    position += speed * dt;
    rotation += rotMomentum * dt;


    //swim(scene, dt);
    generateModelMatrix();

    return true;
}

void fish::swim(Scene &scene, float dt){
    clock_t t;
    t = clock();
    float siner = t / 10000.0f;
    //left or right?
    if (varination == 0){
        position.x += dt;
        rotation.z = M_PI;
        if (abs(position.x) > BOUNDARY){
            varination = 1;
            rotation.z = 0;
        }
    }
    else if (varination == 1) {
        position.x -= dt;
        if (abs(position.x) > BOUNDARY){
            varination = 0;
            rotation.z = M_PI;
        }
    }
    else if (varination == 2){
        position.z +=dt;
        rotation.z = M_PI_2;
        if (abs(position.z) > BOUNDARY){
            varination = 3;
            rotation.z = -M_PI_2;
        }
    }
    else { //varination == 3
        position.z -=dt;
        rotation.z = -M_PI_2;
        if (abs(position.z) > BOUNDARY) {
            varination = 2;
            rotation.z = M_PI_2;
        }
    }
    time += dt;
    //Sometimes, the fish breathes.

    if (time > .3) {
        auto obj = std::make_unique<Bubble>();
        obj->position = position;
        obj->rotation = {0, 0, M_PI};
        if (varination == 0) { //Ensuring the bubbles exit the fish's mouth
            obj->position.x = position.x+1.0f;
            obj->position.z = position.z;
        }
        else if (varination == 1) {
            obj->position.x = position.x-1.0f;
            obj->position.z = position.z;
        }
        else if (varination == 2){
            obj->position.x = position.x;
            obj->position.z = position.z+1.0f;
        }
        else{
            obj->position.x = position.x;
            obj->position.z = position.z-1.0f;
        }

        float randScale = glm::linearRand(0.05f, 0.1f);
        obj->scale = glm::vec3(randScale);
        float randSpeed = glm::linearRand(4.0f, 5.0f);
        obj->speed = {0, randSpeed, 0};
        scene.objects.push_back(move(obj));
        time = 0;
    }
    if ( funny < 1000 ) funny += 1;
    else if ((funny >= 1000) && (((varination < 2) && //Wiggle around
            (abs(position.x) < BOUNDARY - 2.0f) ) || ((varination >= 2) && (abs(position.z) < BOUNDARY - 2.0f) ))){ //To prevent edgecase when wiggling starts too close to boundary.
            float v = sin(siner) / 2;
            if (varination == 0){
                rotation.z = M_PI + v;
            }
            else if (varination == 1) {
                rotation.z = v;
            }
            else if (varination == 2){
                rotation.z = M_PI_2 + v;
            }
            else {
                rotation.z = -M_PI_2 + v;
            }
            if (v == 0) funny +=1;
            std::cout << "wiggle wiggle wiggle" << std::endl;
            if (funny == 1002) funny = 0; //Should do for a while before returning below 10
    }
}

void fish::render(Scene &scene) {
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

void fish::onClick(Scene &scene) {
    std::cout << "Dolphin clicked!" << std::endl;
    age = 10000;
}

