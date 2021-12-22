#include "FishSchool.h"
#include "SmallFish.h"
#include <glm/gtc/random.hpp>
#include <ppgso/ppgso.h>

FishSchool::FishSchool(int fishCount) {
    float x_offset, y_offset, z_offset, phase;
    for(int i = 0; i < fishCount; i++){
        x_offset = glm::linearRand(-3.0f, 3.0f);
        y_offset = glm::linearRand(-3.0f, 3.0f);
        z_offset = glm::linearRand(-3.0f, 3.0f);
        phase = glm::linearRand(0.0f, 2.0f);
        fishes.push_back(std::make_unique<SmallFish>(x_offset, y_offset, z_offset, phase));
    }
    std::cout << "created";

}


bool FishSchool::update(Scene &scene, float dt) {
    time = (float) glfwGetTime();
    position.z += dt;

    generateModelMatrix();
    for(auto& fish : fishes) {
        fish->update(scene, time);
        fish->modelMatrix = modelMatrix * fish->modelMatrix;
        fish->tail->modelMatrix = modelMatrix * fish->tail->modelMatrix;
    }
    return true;
}


void FishSchool::render(Scene &scene) {
    for(auto& fish : fishes) {
        fish->render(scene);
    }

}


