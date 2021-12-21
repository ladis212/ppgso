#include "FishSchool.h"
#include "SmallFish.h"
#include <glm/gtc/random.hpp>
#include <ppgso/ppgso.h>

FishSchool::FishSchool() {
    auto fish = std::make_unique<SmallFish>();
    fishes.push_back(move(fish));
}


bool FishSchool::update(Scene &scene, float dt) {
    for(auto& fish : fishes) {
        fish.
    }
    return true;
}


void FishSchool::render(Scene &scene) {


}


