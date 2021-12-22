// Example gl_scene
// - Introduces the concept of a dynamic scene of objects
// - Uses abstract object interface for Update and Render steps
// - Creates a simple game scene with Player, Asteroid and Space objects
// - Contains a generator object that does not render but adds Asteroids to the scene
// - Some objects use shared resources and all object deallocations are handled automatically
// - Controls: LEFT, RIGHT, "R" to reset, SPACE to fire

#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <cmath>
#include <windows.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <ppgso/ppgso.h>

#include "camera.h"
#include "scene.h"
#include "generator.h"
#include "player.h"
#include "space.h"
#include "skybox.h"
#include "island.h"
#include "dolphin.h"
#include "Bubble.h"
#include "GenericObject.h"
#include "fish.h"
#include "BezierPatch.h"
#include "object.h"
#include "sponge.h"
#include "stingray.h"
#include "Floor.h"
#include "BezierPatch.h"
#include "eye.h"
#include "cave.h"
#include "FishSchool.h"
#include "plank.h"
#include "turtle.h"

//shaders
#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <shaders/point_light_glsl.h>
#include <shaders/bubble_frag_glsl.h>
#include <shaders/bubble_vert_glsl.h>
#include <shaders/underwater_vert_glsl.h>
#include <shaders/underwater_frag_glsl.h>

#include <valarray>

const unsigned int HEIGHT = 720;
const unsigned int WIDTH = 720;

//how many are generated
#define GRASS_MAX 30
#define ROCK_MAX 40
#define CORAL_MAX 30
#define FISH_MAX 10
#define SCHOOL_MAX 10

//Boundaries?
#define HORIZON 90.0f

struct Keyframe {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    float timeToTake;
};

struct CameraFrame{
    glm::vec3 position;
    glm::vec3 target;
    float timeToTake;
};

template<typename Object>
void linearBetweenKeyframes(std::unique_ptr<Object> &object, Keyframe A, Keyframe B, float timeSinceA){
    float t = timeSinceA / B.timeToTake;
    object->position = glm::lerp(A.position, B.position, t);
    object->rotation = glm::lerp(A.rotation, B.rotation, t);
    object->scale = glm::lerp(A.scale, B.scale, t);

}
void CameraLinFrames(std::unique_ptr<Camera> &cam, CameraFrame A, CameraFrame B, float timeSinceA){
    float t = timeSinceA / B.timeToTake;
    cam->position = glm::lerp(A.position, B.position, t);
    cam->target = glm::lerp(A.target, B.target, t);
}

class SceneWindow : public ppgso::Window {
private:
  Scene scene;
  bool animate = true;
  std::unique_ptr<Skybox> skybox;
  std::unique_ptr<Island> island;
  std::unique_ptr<GenericObject> shark;
  std::unique_ptr<Dolphin>dolphin1;
  std::unique_ptr<Dolphin>dolphin2;
  std::unique_ptr<Bubble> bubble;
  std::unique_ptr<ppgso::Texture> skybox_alt_texture;

  std::unique_ptr<Eye> leye;
  std::unique_ptr<Eye> reye;
  std::unique_ptr<Cave> cave;

  std::unique_ptr<GenericObject> grass1;
  std::unique_ptr<GenericObject> grass2;

  ////Coral Reef
  std::unique_ptr<GenericObject> cor1;
  std::unique_ptr<GenericObject> cor2;
  std::unique_ptr<GenericObject> cor3;


  ////Rock Arch
  std::unique_ptr<GenericObject>rockAL0;
  std::unique_ptr<GenericObject>rockAR0;
  std::unique_ptr<GenericObject>rockAL1;
  std::unique_ptr<GenericObject>rockAR1;
  std::unique_ptr<GenericObject>rockAL2;
  std::unique_ptr<GenericObject>rockAR2;
  std::unique_ptr<GenericObject>rockAR3;

  ////Rock Pillar 1
  std::unique_ptr<GenericObject>rock1p1;
  std::unique_ptr<GenericObject>rock2p1;
  std::unique_ptr<GenericObject>rock3p1;

  ////Rock Pillar 2
  std::unique_ptr<GenericObject>rock1p2;
  std::unique_ptr<GenericObject>rock2p2;
  std::unique_ptr<GenericObject>rock3p2;

  ////Rock Pillar 3
  std::unique_ptr<GenericObject>rock1p3;
  std::unique_ptr<GenericObject>rock2p3;
  std::unique_ptr<GenericObject>rock3p3;

  ////Low Poly
  //std::unique_ptr<fish>fish_l1;
  //std::unique_ptr<fish>fish_l2;

  ///Sponges
  std::unique_ptr<Sponge>sponge;
  std::unique_ptr<Sponge>sponge1;
  std::unique_ptr<Sponge>sponge2;

  std::unique_ptr<Stingray>stingray;


    std::unique_ptr<BezierPatch> sea;
    std::unique_ptr<Floor> floor;
  std::vector<Keyframe> octopusKeyframes {
      {
          .position={10,-1,0},
          .rotation={-M_PI_2,0,M_PI_2},
          .scale={.05,.05,.05},
          .timeToTake=0
      },
      {
          .position={0,-1,10},
          .rotation={-M_PI_2, 0, 0},
          .scale={.05,.05,.05},
          .timeToTake=1
      },
      {
          .position={-10,-1,0},
          .rotation={-M_PI_2, 0, -M_PI_2},
          .scale={.05,.05,.05},
          .timeToTake=1
      },
      {
          .position={0,-1,-10},
          .rotation={-M_PI_2, 0, -M_PI},
          .scale={.05,.05,.05},
          .timeToTake=1
      },
      {
              .position={10,-1,0},
              .rotation={-M_PI_2,0,-M_PI - M_PI_2},
              .scale={.05,.05,.05},
              .timeToTake=1
      }
  };

  std::vector<CameraFrame> Camframes{
          { //ACT 1: Spin around the surface ~10s
            .position ={0.0f, 20.0f, 20.0f},
            .target = {0, 5.0f, 0},
            .timeToTake = 3.0f,
          },
          {
             .position ={20.0f, 20.5f, 0.0f},
             .target = {0, 5.0f, 0},
             .timeToTake = 3.0f,
          },
          {
             .position ={0.0f, 20.0f, -20.0f},
             .target = {0, 5.0f, 0},
             .timeToTake = 3.0f,
          },
          {
             .position ={-20.0f, 20.0f, 0.0f},
             .target = {0, 5.0f, 0},
             .timeToTake = 3.0f,
          },
          {
             .position ={0.0f, 15.0f, 20.0f},
             .target = {0, 5.0f, 0},
             .timeToTake = 2.0f,
          }, //ACT 1 End... 10s - 10s
          { //ACT 2 - Advance towards the island. The island is at: 20 0 15, use 4 keyframes, roughly 8 sec (to make it still smooth)
                  .position ={10.0f, 14.0f, 10.0f},
                  .target = {5.0f, 5, 3.75f},
                  .timeToTake = 2.0f,
          },
          { //ACT 3 - kruzit okolo ostrova - pomaly - trochu vyvyseny pre efekt.
                  .position ={20.0f, 13.0f, 0.0f}, //pred
                  .target = {20, 3, 15},
                  .timeToTake = 3.0f,
          },
          {
                  .position ={0.0f, 13.0f, 15.0f},//vedla
                  .target = {20, 3, 15},
                  .timeToTake = 3.0f,
          },
          {
                  .position ={20.0f, 13.0f, 30.0f}, //za
                  .target = {20, 3, 15},
                  .timeToTake = 3.0f,
          },
          {
                  .position ={40.0f, 13.0f, 15.0f}, //vedla
                  .target = {20, 3, 15},
                  .timeToTake = 3.0f,
          },
          {
                  .position ={20.0f, 13.0f, 0.0f},
                  .target = {20, 3, 15},
                  .timeToTake = 3.0f,
          }, //ACT 3 - END - 15s - 35s
          { //ACT 4 - ponor pod morsku hladinu - pomaly, 30s
                  .position ={20.0f, 13.0f, 0.0f}, //navrat na 0, 0 - raja
                  .target = {0, 0, 0},
                  .timeToTake = 5.0f,
          },
          {
                  .position ={20.0f, 10.0f, 0.0f},
                  .target = {0, -2.5f, 0},
                  .timeToTake = 1.0f,
          },
          {
                  .position ={20.0f, 5.0f, 0.0f},
                  .target = {0, -5, 0},
                  .timeToTake = 1.0f,
          },
          {
                  .position ={20.0f, 0.0f, 0.0f},
                  .target = {0, -7.5f, 0},
                  .timeToTake = 1.0f,
          },
          {
                  .position ={20.0f, -5.0f, 0.0f},
                  .target = {0, -10, 0},
                  .timeToTake = 2.0f, //10s
          },
          { //Goes to the central area of a quadrant
                  .position ={20.0f, -10.0f, 0.0f},
                  .target = {10, -13.5, 10},
                  .timeToTake = 2.0f,
          },
          {
                  .position ={20.0f, -10.0f, 0.0f},
                  .target = {20.0f, -17.0f, 20.0f},
                  .timeToTake = 3.0f, //15s
          }, //arrives at quadrant 1  - 25s - act end - 60s
          { //ACT 5 - Quadrant 1
                  .position ={50.0f, -10.0f, 30.0f},
                  .target = {20.0f, -17.0f, 20.0f},
                  .timeToTake = 3.33f,
          },
          {
                  .position ={30.0f, -10.0f, 50.0f},
                  .target = {20.0f, -17.0f, 20.0f},
                  .timeToTake = 3.33f,
          },
          {
                  .position ={0.0f, -10.0f, 30.0f},
                  .target = {20.0f, -17.0f, 20.0f},
                  .timeToTake = 3.34f,
          },
          {
                  .position ={20.0f, -10.0f, 0.0f},
                  .target = {20.0f, -17.0f, 20.0f},
                  .timeToTake = 4.0f,
          }, //ACT end - 15s -75s
          { //ACT 6 intermission
                  .position ={0.0f, -12.0f, 0.0f},
                  .target = {0.0f, -17.0f, 20.0f},
                  .timeToTake = 2.5f,
          },
          {
                  .position ={-20.0f, -12.0f, 0.0f},
                  .target = {-20.0f, -17.0f, 20.0f},
                  .timeToTake = 2.5f,
          }, //80s
          { //ACT 7 - Quadrant 2
                  .position ={-40.0f, -12.0f, 20.0f},
                  .target = {-20.0f, -17.0f, 20.0f},
                  .timeToTake = 3.33f,
          },
          {
                  .position ={-20.0f, -12.0f, 40.0f},
                  .target = {-20.0f, -17.0f, 20.0f},
                  .timeToTake = 3.33f,
          },
          {
                  .position ={0.0f, -12.0f, 20.0f},
                  .target = {-20.0f, -17.0f, 20.0f},
                  .timeToTake = 3.34f,
          },
          {
                  .position ={-20.0f, -12.0f, 0.0f},
                  .target = {-20.0f, -17.0f, 20.0f},
                  .timeToTake = 4.0f,
          }, //95s
          { //quadrant 3 transmission
                  .position ={-15.0f, -12.0f, -0.0f},
                  .target = {-20.0f, -17.0f, 0.0f},
                  .timeToTake = 3.0f,
          },
          {
                 .position ={-20.0f, -12.0f, 0.0f},
                 .target = {-20.0f, -17.0f, -20.0f},
                 .timeToTake = 4.0f,
          }, //100s
          { //QUADRANT 3
                  .position ={-50.0f, -12.0f, -30.0f},
                  .target = {-20.0f, -17.0f, -20.0f},
                  .timeToTake = 3.33f,
          },
          {
                  .position ={-30.0f, -12.0f, -50.0f},
                  .target = {-20.0f, -17.0f, -20.0f},
                  .timeToTake = 3.33f,
          },
          {
                  .position ={0.0f, -12.0f, -20.0f},
                  .target = {-20.0f, -17.0f, -20.0f},
                  .timeToTake = 3.34f,
          },
          {
                  .position ={-20.0f, -12.0f, 0.0f},
                  .target = {-20.0f, -17.0f, -20.0f},
                  .timeToTake = 4.0f,
          }, //115s
          { //transition cez ostatne quadranty
                  .position ={-7.5f, -12.0f, 0.0f},
                  .target = {-20.0f, -17.0f, 20.0f},
                  .timeToTake = 2.0f,
          },
          {
                  .position ={7.5f, -12.0f, 0.0f},
                  .target = {20.0f, -17.0f, 20.0f},
                  .timeToTake = 2.0f,
          },
          {
                  .position ={15.0f, -12.0f, 15.0f},
                  .target = {20.0f, -17.0f, -20.0f},
                  .timeToTake = 2.0f,
          },
          { //QUADRANT 4
                  .position ={0.0f, -12.0f, -20.0f},
                  .target = {20.0f, -17.0f, -20.0f},
                  .timeToTake = 3.33f,
          },
          {
                  .position ={20.0f, -12.0f, -40.0f},
                  .target = {20.0f, -17.0f, -20.0f},
                  .timeToTake = 3.33f,
          },
          {
                  .position ={40.0f, -12.0f, -20.0f},
                  .target = {20.0f, -17.0f, -20.0f},
                  .timeToTake = 3.34f,
          },
          {
                  .position ={20.0f, -12.0f, 0.0f},
                  .target = {20.0f, -17.0f, -20.0f},
                  .timeToTake = 4.0f,
          },

  };

  bool surfaceState = true;

  /*!
   * Reset and initialize the game scene
   * Creating unique smart pointers to objects that are stored in the scene object list
   */
  void initScene() {
      skybox_alt_texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("skybox_ocean.bmp"));

      scene.objects.clear();
      scene.lightDirection = {-1, 1, 0};
      // Create a camera
      auto camera = std::make_unique<Camera>(60.0f, 1.0f, 0.1f, 400.0f);



      camera->position.z = -15.0f;
      scene.camera = move(camera);

      for (int i = 0; i < SCHOOL_MAX; i++) {
          auto fishflock = std::make_unique<FishSchool>(glm::linearRand(5, 12));
          fishflock->position = {glm::linearRand(-HORIZON, HORIZON), glm::linearRand(-5.0f, -15.0f), glm::linearRand(-HORIZON, HORIZON)};
          scene.objects.push_back(move(fishflock));
      }
      skybox = std::make_unique<Skybox>();
      skybox->scale = {200, 200, 200};
      //scene.objects.push_back(move(skybox));
      island = std::make_unique<Island>();
      island->scale = {.10, .10, .10};
      island->position = {20, 0, 15};

      sea = std::make_unique<BezierPatch>();
      floor = std::make_unique<Floor>();
      floor->position.y = -20;
      //bubble = std::make_unique<Bubble>();
      //bubble->scale = {10, 10, 10};
      //bubble->rotation = {0, 0, M_PI};
      //scene.objects.push_back(move(island));
      //skybox->position = {0, 10, 0};
      //auto generator = std::make_unique<Generator>();
      //generator->position.y = -10.0f;
      //scene.objects.push_back(move(generator));
      //dolphin = std::make_unique<Dolphin>();
      shark = std::make_unique<GenericObject>("shark\\12960_Shark_v2_L1.obj", "shark\\12960_Shark_diff_v2.bmp", underwater_vert_glsl, underwater_frag_glsl);
      shark->scale = {.05, .05, .05};
      shark->rotation.y = M_PI_2;
      //skybox->update(scene, 0);
      // Add generator to scene
      //auto generator = std::make_unique<Generator>();
      //generator->position.y = 10.0f;
      //scene.objects.push_back(move(generator));
      dolphin1 = std::make_unique<Dolphin>();
      dolphin1->scale = {.03, .03, .03};
      dolphin1->position = {-7, -4, -15};
      dolphin2 = std::make_unique<Dolphin>();
      dolphin2->scale = {.02, .02, .02};
      dolphin2->position = {0, -3, 0};


      /////// high poly fish
      float s = 0.0f; //Container to equalize size ratio across scale
      ///* ORANGE FISH *///
      for(int i = 0; i < FISH_MAX; i++){
          auto new_fish = std::make_unique<fish>("fishes\\fish.obj", "fishes\\fish.bmp", underwater_vert_glsl, underwater_frag_glsl,i);
          new_fish->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
          s = glm::linearRand(.03f, .1f);
          new_fish->scale = glm::vec3(s);
          new_fish->rotation.x = -M_PI_2;
          scene.objects.push_back(move(new_fish));
      }

      ///* BLUE TANG *///

      for(int i = 0; i < FISH_MAX; i++){
          auto new_fish = std::make_unique<fish>("fishes\\bluetang.obj", "fishes\\bluetang.bmp", underwater_vert_glsl, underwater_frag_glsl,i + FISH_MAX);
          new_fish->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
          s = glm::linearRand(.05f, .2f);
          new_fish->scale = glm::vec3(s);
          new_fish->rotation.y = -M_PI_2;
          new_fish->rotation.x = -M_PI_2;
          scene.objects.push_back(move(new_fish));
      }

      ///*CHROMIS*///

      for(int i = 0; i < FISH_MAX; i++){
          auto new_fish = std::make_unique<fish>("fishes\\chromis.obj", "fishes\\chromis.bmp", underwater_vert_glsl, underwater_frag_glsl,i+ 2* FISH_MAX);
          new_fish->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
          s = glm::linearRand(.05f, .2f);
          new_fish->scale = glm::vec3(s);
          new_fish->rotation.y = -M_PI_2;
          new_fish->rotation.x = -M_PI_2;
          scene.objects.push_back(move(new_fish));
      }

      ////// low poly fish
      //fish_l1 = std::make_unique<fish>("fishes\\finalfish.obj", "fishes\\finalfish.bmp", diffuse_vert_glsl, diffuse_frag_glsl);
      //fish_l2 = std::make_unique<fish>("fishes\\finalfish.obj", "fishes\\specialfinalfish.bmp", diffuse_vert_glsl, diffuse_frag_glsl);

      ///Planks
      for (int i = 0; i < 10; i++){
          auto plank = std::make_unique<Plank>();
         plank->position = {glm::linearRand(-20.0, 20.0), 0, glm::linearRand(-20.0, 20.0)};
         scene.objects.push_back(move(plank));
     }



      ////Stingray
      stingray = std::make_unique<Stingray>();
      stingray->position = {0, -17.0f, 0};
      stingray->scale = {5, 5, 5};

      ///* QUADRANT 1 + + *///
      ////Cave
      cave = std::make_unique<Cave>();
      cave->position = {20.0f, -17.0f, 20.0f};
      cave->scale = {7, 7, 7};
      cave->rotation.z = M_PI_2;
      //// Sponge
      sponge = std::make_unique<Sponge>();
      sponge->scale = {.07, .07, .07};
      sponge->position = {19.0f, -8.0f, 23.5f};
      ///Sponge 2
      sponge1 = std::make_unique<Sponge>();
      sponge1->scale = {.1, .1, .1};
      sponge1->position = {12.0f, -12.0f, 20.5f};
      sponge1->rotation.x = M_PI_2;
      ///*PATHWAY Q1 --> Q2///
      auto new_rock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\darkrock.bmp", underwater_vert_glsl, underwater_frag_glsl);
      new_rock->scale = {3.5f, 3.5f, 3.5f};
      new_rock->rotation.z = -M_PI_2;
      new_rock->rotation.x = -M_PI_2;
      new_rock->position = {6.0f, -19.5f, 22.0f};
      scene.objects.push_back(move(new_rock));

      auto first_grass = std::make_unique<GenericObject>("coral\\Grass.obj", "coral\\Grass.bmp", underwater_vert_glsl, underwater_frag_glsl);
      first_grass->scale = {30, 30, 30};
      first_grass->position = {7.0f, -19.5f, 20.5f};
      scene.objects.push_back(move(first_grass));

      auto second_grass = std::make_unique<GenericObject>("coral\\Grass.obj", "coral\\SeaGrass.bmp", underwater_vert_glsl, underwater_frag_glsl);
      second_grass->scale = {40, 40, 40};
      second_grass->position = {1.5f, -19.5f, 24.5f};
      scene.objects.push_back(move(second_grass));

      auto brain_coral = std::make_unique<GenericObject>("coral\\braincoral.obj", "coral\\braincoral.bmp", underwater_vert_glsl, underwater_frag_glsl);
      brain_coral->position = {-4.0f, -19.0f, 21.5f};
      brain_coral->scale = {.55f, .55f, .55f};
      brain_coral->rotation.x = -M_PI_2;
      scene.objects.push_back(move(brain_coral));

      auto tree_coral = std::make_unique<GenericObject>("coral\\treecoral.obj", "coral\\treecoralcustom.bmp", underwater_vert_glsl, underwater_frag_glsl);
      tree_coral->position = {-6.5f, -19.0f, 26.5f};
      scene.objects.push_back(move(tree_coral));

      auto second_rock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\rock2_2.bmp", underwater_vert_glsl, underwater_frag_glsl);
      second_rock->scale = {3.5f, 3.5f, 3.5f};
      second_rock->rotation.z = M_PI_2;
      second_rock->rotation.y = -M_PI_2;
      second_rock->position = {-1.0f, -19.7f, 30.0f};
      scene.objects.push_back(move(second_rock));

      auto third_rock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\undersearock.bmp", underwater_vert_glsl, underwater_frag_glsl);
      third_rock->scale = {3.5f, 3.5f, 3.5f};
      third_rock->rotation.z = M_PI_2;
      third_rock->rotation.x = -M_PI_2;
      third_rock->position = {-8.0f, -19.5f, 27.0f};
      scene.objects.push_back(move(third_rock));


      ///* QUADRANT 2 - + *///
      ///Rock Arch
      //endpoints
      rockAL0 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\sandstone.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rockAL0->position = {-25.0f, -19.5f, 15.0f};
      rockAL0->scale = {4.5, 3, 4.5};
      rockAL1 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\brownrock.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rockAL1->position = {-23.33f, -17.5f, 16.67f};
      rockAL1->scale = {5, 3, 5};
      rockAL2 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\sandstone.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rockAL2->position = {-21.67f, -15.5f, 18.33f};
      rockAL2->scale = {5.5f, 3, 5.5f};
      rockAL2->rotation.x = -M_PI_2;
 //     rockAL1->rotation.y =  M_PI_4;
      rockAR3 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\RockTexture.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rockAR3->position = {-20.0f, -13.5f, 20.0f};
      rockAR3->scale = {6, 2, 6};
 //     rockAL2->rotation.y =  M_PI_2;
      rockAR2 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\grayrock.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rockAR2->position = {-18.33f, -15.5f, 21.67f};
      rockAR2->scale = {5.5f, 3, 5.5f};
      rockAL2->rotation.y = M_PI;
      rockAR1 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\rock2_1.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rockAR1->position = {-16.67f, -17.5f, 23.33f};
      rockAR1->scale = {5, 3, 5};
  //    rockAR1->rotation.y =  -M_PI_4;
      rockAR0 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\Rock-Texture-Surface.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rockAR0->rotation.x = -M_PI;
      rockAR0->position = {-15.0f, -19.5f, 25.0f};
      rockAR0->scale = {4.5f, 3, 4.5f};
      ///Sea Grass
      grass1 = std::make_unique<GenericObject>("coral\\Grass.obj", "coral\\Grass.bmp", underwater_vert_glsl, underwater_frag_glsl);
      grass1->scale = {35, 35, 35};
      grass1->position = {-23.0f, -19.5f, 13.0f};
      grass2 = std::make_unique<GenericObject>("coral\\Grass.obj", "coral\\SeaGrass.bmp", underwater_vert_glsl, underwater_frag_glsl);
      grass2->scale = {37, 37, 37};
      grass2->position = {-13.0f, -19.5f, 23.0f};

      ///Turtle
      auto turtle = std::make_unique<Turtle>();
      turtle->position = {-20.0f,-19.25f,20.0f};
      turtle->scale = {.07, .07, .07};
      turtle->rotation.x = -M_PI_2;
      turtle->rotation.z = M_PI_4;
      scene.objects.push_back(move(turtle));

      ///* QUADRANT 3 - - *///
      ///Coral Reef
      cor1 = std::make_unique<GenericObject>("coral\\braincoral.obj", "coral\\braincoral.bmp", underwater_vert_glsl, underwater_frag_glsl);
      cor1->position = {-30, -19.0f, -15};
      cor1->scale = {.65f, .65f, .65f};
      cor1->rotation.x = -M_PI_2;
      cor2 = std::make_unique<GenericObject>("coral\\v1coral.obj", "coral\\v1coral.bmp", underwater_vert_glsl, underwater_frag_glsl);
      cor2->position = {-25, -19.0f, -25};
      cor2->scale = {.4f, .4f, .4f};
      cor2->rotation.x = -M_PI_2;
      cor3 = std::make_unique<GenericObject>("coral\\treecoral.obj", "coral\\treecoralcustom.bmp", underwater_vert_glsl, underwater_frag_glsl);
      cor3->scale = {1.5f, 1.5f, 1.5f};
      cor3->position = {-10, -19.0f, -30};
      ///Sponge 3
      sponge2 = std::make_unique<Sponge>();
      sponge2->scale = {.1, .1, .1};
      sponge2->position = {-22.5f, -20.0f, -12.0f};

      ///* QUADRANT 4 + - *///
      ///Triple Rock Columns
      //pillar 1
      rock1p1 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\rock2_2.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rock1p1->position = {15.5f, -19.5f, -8.0f};
      rock1p1->scale = {5, 5, 5};
      rock1p1->rotation.x = M_PI_2;
      rock2p1 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\brownrock.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rock2p1->position = {15.5f, -15.0f, -8.0f};
      rock2p1->scale = {4.5, 4.5, 4.5};
      rock2p1->rotation.x = M_PI_2;
      rock2p1->rotation.z = M_PI;
      rock3p1 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\Rock-Texture-Surface.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rock3p1->position = {15.5f, -11.5f, -8.0f};
      rock3p1->scale = {3.5, 3.5, 3.5};
      rock3p1->rotation.x = M_PI_2;
      rock3p1->rotation.z = -M_PI_2;

      //pillar2
      rock1p2 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\darkrock.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rock1p2->position = {22.0f, -19.5f, -28.0f};
      rock1p2->scale = {7, 7, 7};
      rock1p2->rotation.x = -M_PI_2;
      rock2p2 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\RockTexture.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rock2p2->position = {22.0f, -14.0f, -28.0f};
      rock2p2->scale = {5.5, 5.5, 5.5};
      rock2p2->rotation.x = M_PI_2;
      rock2p2->rotation.y = M_PI;
      rock3p2 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\darkrock.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rock3p2->position = {22.0f, -11.0f, -28.0f};
      rock3p2->scale = {3.5, 3.5, 3.5};
      rock3p2->rotation.z = M_PI_2;
      rock3p2->rotation.x = M_PI;

      //pillar3
      rock1p3 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\undersearock.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rock1p3->position = {35.0f, -19.5f, -16.0f};
      rock1p3->scale = {6, 6, 6};
      rock1p3->rotation.x = M_PI_2;
      rock1p3->rotation.y = -M_PI_2;
      rock2p3 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\grayrock.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rock2p3->position = {35.0f, -16.5f, -16.0f};
      rock2p3->scale = {4, 4, 4};
      rock2p3->rotation.x = -M_PI_2;
      rock2p3->rotation.y = M_PI_2;
      rock3p3 = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\RockTexture.bmp", underwater_vert_glsl, underwater_frag_glsl);
      rock3p3->position = {35.0f, -14.5f, -16.0f};
      rock3p3->scale = {3, 3, 3};
      rock3p3->rotation.x = -M_PI;
      rock3p3->rotation.z = M_PI_2;

      int variation = 0;
      float ratio = 0.0f;
      ///* PROCEDURAL SCENERY *///
      //Corals - Generate 20 of them.
      for(int i = 0; i < CORAL_MAX; i++){
          variation = glm::linearRand(1, 3);
          if (variation == 1){ //Brain Coral
              auto new_coral = std::make_unique<GenericObject>("coral\\braincoral.obj", "coral\\braincoral.bmp", underwater_vert_glsl, underwater_frag_glsl);
              ratio = glm::linearRand(.1f, .6f);
              new_coral->scale = glm::vec3(ratio);
              new_coral->rotation.x = -M_PI_2;
              new_coral->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(new_coral));
          }
          else if (variation == 2){ //Coral V1
              auto new_coral =  std::make_unique<GenericObject>("coral\\v1coral.obj", "coral\\v1coral.bmp", underwater_vert_glsl, underwater_frag_glsl);
              ratio = glm::linearRand(.01f, .2f);
              new_coral->scale = glm::vec3(ratio);
              new_coral->rotation.x = -M_PI_2;
              new_coral->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(new_coral));
          }
          else { //variation = 3 - Tree Coral
              auto new_coral = std::make_unique<GenericObject>("coral\\treecoral.obj", "coral\\treecoralcustom.bmp", underwater_vert_glsl, underwater_frag_glsl);
              ratio = glm::linearRand(.3f, 1.2f);
              new_coral->scale = glm::vec3(ratio);
              new_coral->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(new_coral));
          }
      }
      //Rocks - Generate 30 of them
      glm::vec3 rotisserie = {0, 0, 0}; //Triad that picks out rotation variation between M_PI, M_PI_2, -M_PI_2 and 0
      ratio = 0.0f;
      for(int i = 0; i < ROCK_MAX; i++){
          variation = glm::linearRand(1, 9); //texture picker
          rotisserie = {glm::linearRand(0, 3),glm::linearRand(0, 3),glm::linearRand(0, 3)};

          //TODO: Easier way to do this?? Really only thing that changes in each Variation is the texture of the rock.
          if (variation == 1){
              auto nRock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\brownrock.bmp", underwater_vert_glsl, underwater_frag_glsl);
              nRock->rotation = {rotisserie.x * M_PI_2, rotisserie.y * M_PI_2, rotisserie.z * M_PI_2}; //returns 0, M_PI_2, M_PI, or 3*M_PI_2 = - M_PI
              ratio = glm::linearRand(.5f, 8.0f);
              nRock->scale = glm::vec3(ratio);
              nRock->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(nRock));
          }
          else if (variation == 2){
              auto nRock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\darkrock.bmp", underwater_vert_glsl, underwater_frag_glsl);
              nRock->rotation = {rotisserie.x * M_PI_2, rotisserie.y * M_PI_2, rotisserie.z * M_PI_2}; //returns 0, M_PI_2, M_PI, or 3*M_PI_2 = - M_PI
              ratio = glm::linearRand(.5f, 8.0f);
              nRock->scale = glm::vec3(ratio);
              nRock->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(nRock));
          }
          else if (variation == 3){
              auto nRock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\grayrock.bmp", underwater_vert_glsl, underwater_frag_glsl);
              nRock->rotation = {rotisserie.x * M_PI_2, rotisserie.y * M_PI_2, rotisserie.z * M_PI_2}; //returns 0, M_PI_2, M_PI, or 3*M_PI_2 = - M_PI
              ratio = glm::linearRand(.5f, 8.0f);
              nRock->scale = glm::vec3(ratio);
              nRock->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(nRock));
          }
          else if (variation == 4){
              auto nRock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\rock2_1.bmp", underwater_vert_glsl, underwater_frag_glsl);
              nRock->rotation = {rotisserie.x * M_PI_2, rotisserie.y * M_PI_2, rotisserie.z * M_PI_2}; //returns 0, M_PI_2, M_PI, or 3*M_PI_2 = - M_PI
              ratio = glm::linearRand(.5f, 8.0f);
              nRock->scale = glm::vec3(ratio);
              nRock->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(nRock));
          }
          else if (variation == 5){
              auto nRock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\rock2_2.bmp", underwater_vert_glsl, underwater_frag_glsl);
              nRock->rotation = {rotisserie.x * M_PI_2, rotisserie.y * M_PI_2, rotisserie.z * M_PI_2}; //returns 0, M_PI_2, M_PI, or 3*M_PI_2 = - M_PI
              ratio = glm::linearRand(.5f, 8.0f);
              nRock->scale = glm::vec3(ratio);
              nRock->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(nRock));
          }
          else if (variation == 6){
              auto nRock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\RockTexture.bmp", underwater_vert_glsl, underwater_frag_glsl);
              nRock->rotation = {rotisserie.x * M_PI_2, rotisserie.y * M_PI_2, rotisserie.z * M_PI_2}; //returns 0, M_PI_2, M_PI, or 3*M_PI_2 = - M_PI
              ratio = glm::linearRand(.5f, 8.0f);
              nRock->scale = glm::vec3(ratio);
              nRock->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(nRock));
          }
          else if (variation == 7){
              auto nRock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\Rock-Texture-Surface.bmp", underwater_vert_glsl, underwater_frag_glsl);
              nRock->rotation = {rotisserie.x * M_PI_2, rotisserie.y * M_PI_2, rotisserie.z * M_PI_2}; //returns 0, M_PI_2, M_PI, or 3*M_PI_2 = - M_PI
              ratio = glm::linearRand(.5f, 8.0f);
              nRock->scale = glm::vec3(ratio);
              nRock->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(nRock));
          }
          else if (variation == 8){
              auto nRock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\sandstone.bmp", underwater_vert_glsl, underwater_frag_glsl);
              nRock->rotation = {rotisserie.x * M_PI_2, rotisserie.y * M_PI_2, rotisserie.z * M_PI_2}; //returns 0, M_PI_2, M_PI, or 3*M_PI_2 = - M_PI
              ratio = glm::linearRand(.5f, 8.0f);
              nRock->scale = glm::vec3(ratio);
              nRock->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(nRock));
          }
          else { //variation == 9
              auto nRock = std::make_unique<GenericObject>("rocks\\RockPackByPava.obj", "rocks\\undersearock.bmp", underwater_vert_glsl, underwater_frag_glsl);
              nRock->rotation = {rotisserie.x * M_PI_2, rotisserie.y * M_PI_2, rotisserie.z * M_PI_2}; //returns 0, M_PI_2, M_PI, or 3*M_PI_2 = - M_PI
              ratio = glm::linearRand(.5f, 8.0f);
              nRock->scale = glm::vec3(ratio);
              nRock->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(nRock));
          } //all of this is a total programmer crime
      }
      variation = 0;
      ratio = 0.0f;
      //grass - 20 instances
      for (int i = 0; i < GRASS_MAX; i++){
          variation = glm::linearRand(1, 2);
          if (variation == 1){
              auto third_grass = std::make_unique<GenericObject>("coral\\Grass.obj", "coral\\Grass.bmp", underwater_vert_glsl, underwater_frag_glsl);
              ratio = glm::linearRand(20.0f, 50.0f);
              third_grass->scale = glm::vec3(ratio);
              third_grass->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(third_grass));
          }
          else {
              auto third_grass = std::make_unique<GenericObject>("coral\\Grass.obj", "coral\\SeaGrass.bmp", underwater_vert_glsl, underwater_frag_glsl);
              ratio = glm::linearRand(20.0f, 50.0f);
              third_grass->scale = glm::vec3(ratio);
              third_grass->position = {glm::linearRand(-HORIZON, HORIZON), -19.5, glm::linearRand(-HORIZON, HORIZON)};
              scene.objects.push_back(move(third_grass));
          }
      }


      ////Beautiful Eyes
      leye = std::make_unique<Eye>("eye\\lefteye.bmp", bubble_vert_glsl, texture_frag_glsl);
      //leye = std::make_unique<Eye>("eye\\lefteye.bmp", bubble_vert_glsl, point_light_glsl);
      leye->position = {19, -17.0f, 17};
      leye->scale = {.5, .5, .5};
      reye = std::make_unique<Eye>("eye\\righteye.bmp", bubble_vert_glsl, texture_frag_glsl);
      reye->position = {17, -17.0f, 17};
      reye->scale = {.5, .5, .5};
  }

public:
  /*!
   * Construct custom game window
   */
  SceneWindow() : Window{"gl9_scene", WIDTH, HEIGHT} {
    //hideCursor();
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    // Initialize OpenGL state
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // transparent blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable polygon culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);



    initScene();


  }


  glm::vec3 directions = {0, 0, 0};
  void onKey(int key, int scanCode, int action, int mods) override {
    scene.keyboard[key] = action;

    // Reset
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
      initScene();
    }

    // Pause
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
      animate = !animate;
    }

    if (key == GLFW_KEY_W) {
        if(action == GLFW_PRESS) directions.z = 1;
        if(action == GLFW_RELEASE) directions.z = 0;
    }

    if (key == GLFW_KEY_S) {

        if(action == GLFW_PRESS) directions.z = -1;
        if(action == GLFW_RELEASE) directions.z = 0;
    }

    if (key == GLFW_KEY_D) {
        if(action == GLFW_PRESS) directions.x = 1;
        if(action == GLFW_RELEASE) directions.x = 0;
    }

    if (key == GLFW_KEY_A) {
        if(action == GLFW_PRESS) directions.x = -1;
        if(action == GLFW_RELEASE) directions.x = 0;

    }

    if (key == GLFW_KEY_Q) {
        if(action == GLFW_PRESS) directions.y = -1;
        if(action == GLFW_RELEASE)directions.y = 0;

    }

    if (key == GLFW_KEY_E) {
        if(action == GLFW_PRESS) directions.y = 1;
        if(action == GLFW_RELEASE) directions.y = 0;

    }


  }

  /*!
   * Handle cursor position changes
   * @param cursorX Mouse horizontal position in window coordinates
   * @param cursorY Mouse vertical position in window coordinates
   */
  void onCursorPos(double cursorX, double cursorY) override {
    scene.cursor.x = cursorX;
    scene.cursor.y = cursorY;
  }

  /*!
   * Handle cursor buttons
   * @param button Mouse button being manipulated
   * @param action Mouse bu
   * @param mods Mouse
   */

  void onMouseButton(int button, int action, int mods) override {
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
      scene.cursor.left = action == GLFW_PRESS;

      if (scene.cursor.left) {
        // Convert pixel coordinates to Screen coordinates
        double u = (scene.cursor.x / width - 0.5f) * 2.0f;
        double v = - (scene.cursor.y / height - 0.5f) * 2.0f;

        // Get mouse pick vector in world coordinates
        auto direction = scene.camera->cast(u, v);
        auto position = scene.camera->position;

        // Get all objects in scene intersected by ray
        auto picked = scene.intersect(position, direction);

        // Go through all objects that have been picked
        for (auto &obj: picked) {
          // Pass on the click event
          obj->onClick(scene);
        }
      }
    }
    if(button == GLFW_MOUSE_BUTTON_RIGHT) {
      scene.cursor.right = action == GLFW_PRESS;
    }
  }

  /*!
   * Window update implementation that will be called automatically from pollEvents
   */
  float keyframeTime = 0;
  float timestamp = 0;
  int keyframeIndex = 0;
  Keyframe thisKeyframe, nextKeyframe;
  int CamIndex = 0;
  CameraFrame thatKeyframe, followKeyframe;
  float camtime = 0.0f;
  float timest = 0.0f;

  void onIdle() override {

    // Track time
    static auto time = (float) glfwGetTime();

    // Compute time delta
    float dt = animate ? (float) glfwGetTime() - time : 0;

    //skybox->rotMomentum = {1, 0, 1};

    int distance = 20;
    //scene.camera->position = {distance * (sin(time / 2)), 5, distance * (cos(time / 2))};
    skybox->position = scene.camera->position;

    if((scene.camera->position.y < 0 && surfaceState == 1) || (scene.camera->position.y >= 0 && surfaceState == 0)){
        skybox->changeTexture(skybox_alt_texture);
        surfaceState = !surfaceState;
    }

    float speed = 0.5f;
    glm::vec3 direction = glm::normalize(scene.camera->target - scene.camera->position);
    //printf("%d %d %d \n", direction.x, direction.y, direction.z);
    scene.camera->position += (directions ) * speed;

    time = (float) glfwGetTime();

    keyframeTime = time - timestamp;

    thisKeyframe = octopusKeyframes[keyframeIndex % octopusKeyframes.size()];
    nextKeyframe = octopusKeyframes[(keyframeIndex + 1) % octopusKeyframes.size()];
    linearBetweenKeyframes(shark, thisKeyframe, nextKeyframe, keyframeTime);
    if(keyframeTime >= nextKeyframe.timeToTake) {
        keyframeTime = 0;
        keyframeIndex++;
        timestamp = time;
    }
      camtime = time - timest;

      thatKeyframe = Camframes[CamIndex % Camframes.size()];
      followKeyframe = Camframes[(CamIndex + 1) % Camframes.size()];
      CameraLinFrames(scene.camera, thatKeyframe, followKeyframe, camtime);
      if(camtime >= followKeyframe.timeToTake) {
          camtime = 0;
          CamIndex++;
          timest = time;
      }


    // Set gray background
    glClearColor(.5f, .5f, .5f, 0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update and render all objects
    //scene.camera->position = {0, 0, -15 + 3 * sin(time)};

    skybox->update(scene, dt);
    skybox->render(scene);
    island->update(scene, dt);
    island->render(scene);
    shark->update(scene, dt);
    shark->render(scene);
    dolphin1->update(scene, dt);
    dolphin1->render(scene);
    dolphin2->update(scene, dt);
    dolphin2->render(scene);

    floor->update(scene, time);
    floor->render(scene);



    //koraly a rastlinky
    cor1->update(scene,dt);
    cor1->render(scene);
    cor2->update(scene,dt);
    cor2->render(scene);
    cor3->update(scene,dt);
    cor3->render(scene);


    //bubble->update(scene, dt);
    //bubble->render(scene);
    //// Sponges
    sponge->update(scene,dt);
    sponge->render(scene);
    sponge1->update(scene,dt);
    sponge1->render(scene);
    sponge2->update(scene,dt);
    sponge2->render(scene);

    cave->update(scene,dt);
    cave->render(scene);

    ///Rocks
    //Archway
    rockAL0->update(scene, dt);
    rockAL0->render(scene);
    rockAR0->update(scene, dt);
    rockAR0->render(scene);
    rockAL1->update(scene, dt);
    rockAL1->render(scene);
    rockAR1->update(scene, dt);
    rockAR1->render(scene);
    rockAL2->update(scene, dt);
    rockAL2->render(scene);
    rockAR2->update(scene,dt);
    rockAR2->render(scene);
    rockAR3->update(scene,dt);
    rockAR3->render(scene);


    grass1->update(scene,dt);
    grass1->render(scene);
    grass2->update(scene,dt);
    grass2->render(scene);

    //pillar 1
    rock1p1->update(scene,dt);
    rock1p1->render(scene);
    rock2p1->update(scene, dt);
    rock2p1->render(scene);
    rock3p1->update(scene, dt);
    rock3p1->render(scene);

    //pillar 2
    rock1p2->update(scene,dt);
    rock1p2->render(scene);
    rock2p2->update(scene,dt);
    rock2p2->render(scene);
    rock3p2->update(scene,dt);
    rock3p2->render(scene);

    //pillar 3
    rock1p3->update(scene,dt);
    rock1p3->render(scene);
    rock2p3->update(scene,dt);
    rock2p3->render(scene);
    rock3p3->update(scene,dt);
    rock3p3->render(scene);

    leye->update(scene, dt);
    leye->render(scene);
    reye->update(scene, dt);
    reye->render(scene);
    scene.update(dt);
    scene.render();

    stingray->update(scene,dt);
    stingray->render(scene);

    sea->update(scene, time);
    sea->render(scene);
  }
};

int main() {
  // Initialize our window
  SceneWindow window;

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
