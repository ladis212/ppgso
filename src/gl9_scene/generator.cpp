#include <glm/gtc/random.hpp>
#include <ppgso/ppgso.h>

#include "generator.h"
#include "Bubble.h"

bool Generator::update(Scene &scene, float dt) {
  // Accumulate time
  time += dt;

  //Add object to scene when time reaches certain level
  if (time > .01) {
    auto obj = std::make_unique<Bubble>();
    obj->position = position;
    obj->rotation = {0, 0, M_PI};
    obj->position.x += glm::linearRand(-20.0f, 20.0f);
    obj->position.z += glm::linearRand(-4.0f, 4.0f);
    float randScale = glm::linearRand(0.1f, 0.5f);
    obj->scale = glm::vec3(randScale);
    float randSpeed = glm::linearRand(4.0f, 5.0f);
    obj->speed = {0, randSpeed, 0};
    scene.objects.push_back(move(obj));
    time = 0;
    printf("Generated.\n");
  }

  return true;
}

void Generator::render(Scene &scene) {
  // Generator will not be rendered
}
