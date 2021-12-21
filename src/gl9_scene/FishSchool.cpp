#include "FishSchool.h"
#include "SmallFish.h"
#include <glm/gtc/random.hpp>
#include <ppgso/ppgso.h>

FishSchool::FishSchool() {
    auto fish = std::make_unique<SmallFish>();
    fishes.push_back(move(fish));
}


bool FishSchool::update(Scene &scene, float dt) {


    generateModelMatrix();
    for(auto& fish : fishes) {
        fish->update(scene, dt);
        fish->modelMatrix *= modelMatrix;
    }
    return true;
}


void FishSchool::render(Scene &scene) {


}


