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

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
//#include <shaders/point_light_glsl.h>
#include <shaders/underwater_vert_glsl.h>
#include <shaders/underwater_frag_glsl.h>

#include <valarray>

const unsigned int HEIGHT = 720;
const unsigned int WIDTH = 720;

struct Keyframe {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    float timeToTake;
};

int arrA = glm::linearRand(2, 10);
int arrB = glm::linearRand(2, 10);
int arrC = glm::linearRand(2,10);

template<typename Object>
void linearBetweenKeyframes(std::unique_ptr<Object> &object, Keyframe A, Keyframe B, float timeSinceA){
    float t = timeSinceA / B.timeToTake;
    object->position = glm::lerp(A.position, B.position, t);
    object->rotation = glm::lerp(A.rotation, B.rotation, t);
    object->scale = glm::lerp(A.scale, B.scale, t);

}

class SceneWindow : public ppgso::Window {
private:
  Scene scene;
  bool animate = true;
  std::unique_ptr<Skybox> skybox;
  std::unique_ptr<Island> island;
  std::unique_ptr<GenericObject> dolphin;
  std::unique_ptr<Dolphin>dolphin2;
  std::unique_ptr<Bubble> bubble;
  std::unique_ptr<ppgso::Texture> skybox_alt_texture;
  std::unique_ptr<GenericObject> cor1;
  //std::unique_ptr<GenericObject> cor2;
  //std::unique_ptr<GenericObject> cor3;
  //std::unique_ptr<GenericObject> grass1;
  //std::unique_ptr<GenericObject> grass2;
  ////Orange fish
  std::unique_ptr<fish>fish_h1;
  std::unique_ptr<fish>fish_h11;
  std::unique_ptr<fish>fish_h12;
  std::unique_ptr<fish>fish_h13;
  std::unique_ptr<fish>fish_h14;

  ////Blue Tang
  std::unique_ptr<fish>fish_h2;
  std::unique_ptr<fish>fish_h21;
  std::unique_ptr<fish>fish_h22;
  std::unique_ptr<fish>fish_h23;
  std::unique_ptr<fish>fish_h24;

  ////Chromis
  std::unique_ptr<fish>fish_h3;
  std::unique_ptr<fish>fish_h31;
  std::unique_ptr<fish>fish_h32;
  std::unique_ptr<fish>fish_h33;
  std::unique_ptr<fish>fish_h34;

  ////Low Poly
  //std::unique_ptr<fish>fish_l1;
  //std::unique_ptr<fish>fish_l2;
  std::unique_ptr<Sponge>sponge;
  std::unique_ptr<Stingray>stingray;


    std::unique_ptr<BezierPatch> sea;
    std::unique_ptr<Floor> floor;
  std::vector<Keyframe> octopusKeyframes {
      {
          .position={5,0,0},
          .rotation={0,0,M_PI * 2},
          .scale={.05,.05,.05},
          .timeToTake=0
      },
      {
          .position={0,0,5},
          .rotation={0, 0, M_PI + M_PI_2},
          .scale={.05,.05,.05},
          .timeToTake=1
      },
      {
          .position={-5,0,0},
          .rotation={0, 0, M_PI},
          .scale={.05,.05,.05},
          .timeToTake=1
      },
      {
          .position={0,0,-5},
          .rotation={0, 0, M_PI_2},
          .scale={.05,.05,.05},
          .timeToTake=1
      },
      {
              .position={5,0,0},
              .rotation={0,0,0},
              .scale={.05,.05,.05},
              .timeToTake=1
      }





  };

  bool surfaceState = true;

  /*!
   * Reset and initialize the game scene
   * Creating unique smart pointers to objects that are stored in the scene object list
   */
  void initScene() {
      skybox_alt_texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("skybox_ocean.bmp"));

      scene.objects.clear();
      scene.lightDirection = {1, 1, 0};
      // Create a camera
      auto camera = std::make_unique<Camera>(60.0f, 1.0f, 0.1f, 300.0f);
      camera->position.z = -15.0f;
      scene.camera = move(camera);
      // Add space background
      // UNDO
      //scene.objects.push_back(std::make_unique<Space>());
      skybox = std::make_unique<Skybox>();
      skybox->scale = {100, 100, 100};
      //scene.objects.push_back(move(skybox));
      island = std::make_unique<Island>();
      island->scale = {.10, .10, .10};
      island->position = {0, 0, 0};

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
      dolphin = std::make_unique<GenericObject>("dolphin\\10014_dolphin_v2_max2011_it2.obj", "dolphin\\10014_dolphin_v1_Diffuse.bmp", underwater_vert_glsl, underwater_frag_glsl);
      dolphin->scale = {.05, .05, .05};;
      //skybox->update(scene, 0);
      // Add generator to scene
      //auto generator = std::make_unique<Generator>();
      //generator->position.y = 10.0f;
      //scene.objects.push_back(move(generator));
      dolphin2 = std::make_unique<Dolphin>();
      dolphin2->scale = {.05, .05, .05};
      ///// koraly a rastlinky
      cor1 = std::make_unique<GenericObject>("coral\\braincoral.obj", "coral\\braincoral.bmp", underwater_vert_glsl, underwater_frag_glsl);
      cor1->position = {30, -20, -10};
      cor1->scale = {.5f, .5f, .5f};
      cor1->rotation.x = M_PI_2;
      //cor2 = std::make_unique<GenericObject>("coral\\v1coral.obj", "coral\\v1coral.bmp", diffuse_vert_glsl, diffuse_frag_glsl);
      //cor3 = std::make_unique<GenericObject>("coral\\treecoral.obj", "coral\\treecoralcustom.bmp", diffuse_vert_glsl, diffuse_frag_glsl); //Textura je CUSTOM -- nezarucujem, ale povodna bola bugnuta.
      //grass1 = std::make_unique<GenericObject>("coral\\Grass.obj", "coral\\Grass.bmp", diffuse_vert_glsl, diffuse_frag_glsl);
      //grass2 = std::make_unique<GenericObject>("coral\\Grass.obj", "coral\\SeaGrass.bmp", diffuse_vert_glsl, diffuse_frag_glsl);
      /////// high poly fish
      //TODO: Easier way to do this?? Array??
      float s = 0.0f; //Container to equalize size ratio across scale
      ///* ORANGE FISH *///
      fish_h1 = std::make_unique<fish>("fishes\\fish.obj", "fishes\\fish.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h1->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.03f, .1f);
      fish_h1->scale = {s, s, s};
      fish_h1->rotation.x = -M_PI_2;
      fish_h11 = std::make_unique<fish>("fishes\\fish.obj", "fishes\\fish.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h11->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.03f, .1f);
      fish_h11->scale = {s, s, s};
      fish_h11->rotation.x = -M_PI_2;
      fish_h12 = std::make_unique<fish>("fishes\\fish.obj", "fishes\\fish.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h12->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.03f, .1f);
      fish_h12->scale = {s, s, s};
      fish_h12->rotation.x = -M_PI_2;
      fish_h13 = std::make_unique<fish>("fishes\\fish.obj", "fishes\\fish.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h13->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.03f, .1f);
      fish_h13->scale = {s, s, s};
      fish_h13->rotation.x = -M_PI_2;
      fish_h14 = std::make_unique<fish>("fishes\\fish.obj", "fishes\\fish.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h14->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.03f, .1f);
      fish_h14->scale = {s, s, s};
      fish_h14->rotation.x = -M_PI_2;

      ///* BLUE TANG *///
      fish_h2 = std::make_unique<fish>("fishes\\bluetang.obj", "fishes\\bluetang.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h2->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.05f, .2f);
      fish_h2->scale = {s, s, s};
      fish_h2->rotation.y= -M_PI_2;
      fish_h2->rotation.x = -M_PI_2;
      fish_h21 = std::make_unique<fish>("fishes\\bluetang.obj", "fishes\\bluetang.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h21->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.05f, .2f);
      fish_h21->scale = {s, s, s};
      fish_h21->rotation.y= -M_PI_2;
      fish_h21->rotation.x = -M_PI_2;
      fish_h22 = std::make_unique<fish>("fishes\\bluetang.obj", "fishes\\bluetang.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h22->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.05f, .2f);
      fish_h22->scale = {s, s, s};
      fish_h22->rotation.y= -M_PI_2;
      fish_h22->rotation.x = -M_PI_2;
      fish_h23 = std::make_unique<fish>("fishes\\bluetang.obj", "fishes\\bluetang.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h23->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.05f, .2f);
      fish_h23->scale = {s, s, s};
      fish_h23->rotation.y= -M_PI_2;
      fish_h23->rotation.x = -M_PI_2;
      fish_h24 = std::make_unique<fish>("fishes\\bluetang.obj", "fishes\\bluetang.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h24->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.05f, .2f);
      fish_h24->scale = {s, s, s};
      fish_h24->rotation.y= -M_PI_2;
      fish_h24->rotation.x = -M_PI_2;

      ///*CHROMIS*///
      fish_h3 = std::make_unique<fish>("fishes\\chromis.obj", "fishes\\chromis.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h3->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.05f, .2f);
      fish_h3->scale = {s, s, s};
      fish_h3->rotation.y = -M_PI_2;
      fish_h3->rotation.x = -M_PI_2;
      fish_h31 = std::make_unique<fish>("fishes\\chromis.obj", "fishes\\chromis.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h31->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.05f, .2f);
      fish_h31->scale = {s, s, s};
      fish_h31->rotation.y = -M_PI_2;
      fish_h31->rotation.x = -M_PI_2;
      fish_h32 = std::make_unique<fish>("fishes\\chromis.obj", "fishes\\chromis.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h32->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.05f, .2f);
      fish_h32->scale = {s, s, s};
      fish_h32->rotation.y = -M_PI_2;
      fish_h32->rotation.x = -M_PI_2;
      fish_h33 = std::make_unique<fish>("fishes\\chromis.obj", "fishes\\chromis.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h33->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.05f, .2f);
      fish_h33->scale = {s, s, s};
      fish_h33->rotation.y = -M_PI_2;
      fish_h33->rotation.x = -M_PI_2;
      fish_h34 = std::make_unique<fish>("fishes\\chromis.obj", "fishes\\chromis.bmp", underwater_vert_glsl, underwater_frag_glsl);
      fish_h34->position = {glm::linearRand(-20.0f, 20.0f), glm::linearRand(-20.0f, -1.0f), glm::linearRand(-20.0f, 20.0f)};
      s = glm::linearRand(.05f, .2f);
      fish_h34->scale = {s, s, s};
      fish_h34->rotation.y = -M_PI_2;
      fish_h34->rotation.x = -M_PI_2;

      ////// low poly fish
      //fish_l1 = std::make_unique<fish>("fishes\\finalfish.obj", "fishes\\finalfish.bmp", diffuse_vert_glsl, diffuse_frag_glsl);
      //fish_l2 = std::make_unique<fish>("fishes\\finalfish.obj", "fishes\\specialfinalfish.bmp", diffuse_vert_glsl, diffuse_frag_glsl);
      //// Sponge
      sponge = std::make_unique<Sponge>();
      sponge->scale = {.1, .1, .1};
      sponge->position = {0, -20, .2};
      stingray = std::make_unique<Stingray>();
      stingray->position = {0, 10, .2};
      stingray->scale = {5, 5, 5};
     // sgenerator = std::make_unique<SpongeGenerator>();
      //sgenerator->position = sponge->position;
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

    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    //glShadeModel(GL_SMOOTH);

    initScene();


  }

  /*!
   * Handles pressed key when the window is focused
   * @param key Key code of the key being pressed/released
   * @param scanCode Scan code of the key being pressed/released
   * @param action Action indicating the key state change
   * @param mods Additional modifiers to consider
   */
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

    float speed = 0.1f;
    glm::vec3 direction = glm::normalize(scene.camera->target - scene.camera->position);
    //printf("%d %d %d \n", direction.x, direction.y, direction.z);
    scene.camera->position += (directions /** direction*/) * speed;

    time = (float) glfwGetTime();

    keyframeTime = time - timestamp;

    thisKeyframe = octopusKeyframes[keyframeIndex % octopusKeyframes.size()];
    nextKeyframe = octopusKeyframes[(keyframeIndex + 1) % octopusKeyframes.size()];
    linearBetweenKeyframes(dolphin, thisKeyframe, nextKeyframe, keyframeTime);
    if(keyframeTime >= nextKeyframe.timeToTake) {
        keyframeTime = 0;
        keyframeIndex++;
        timestamp = time;
    }
    std::cout << keyframeIndex;
    std::cout << (keyframeIndex + 1) << std::endl;


    //dolphin->position.y = time;


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
    dolphin->update(scene, dt);
    dolphin->render(scene);
    dolphin2->update(scene, dt);
    dolphin2->render(scene);

    floor->update(scene, time);
    floor->render(scene);



    //koraly a rastlinky
    cor1->update(scene,dt);
    cor1->render(scene);
    //cor2->update(scene,dt);
    //cor2->render(scene);
    //cor3->update(scene,dt);
    //cor3->render(scene);
    //grass1->update(scene,dt);
    //grass1->render(scene);
    //grass2->update(scene,dt);
    //grass2->render(scene);

    //ryby
    ///ORANGE FISH
    fish_h1->swim(scene,dt);
    fish_h1->update(scene,dt);
    fish_h1->render(scene);

    fish_h11->swim(scene,dt);
    fish_h11->update(scene,dt);
    fish_h11->render(scene);

    fish_h12->swim(scene,dt);
    fish_h12->update(scene,dt);
    fish_h12->render(scene);

    fish_h13->swim(scene,dt);
    fish_h13->update(scene,dt);
    fish_h13->render(scene);

    fish_h14->swim(scene,dt);
    fish_h14->update(scene,dt);
    fish_h14->render(scene);

      ///BLUE TANG
    fish_h2->swim(scene,dt);
    fish_h2->update(scene,dt);
    fish_h2->render(scene);

   fish_h21->swim(scene,dt);
   fish_h21->update(scene,dt);
   fish_h21->render(scene);

   fish_h22->swim(scene,dt);
   fish_h22->update(scene,dt);
   fish_h22->render(scene);

   fish_h23->swim(scene,dt);
   fish_h23->update(scene,dt);
   fish_h23->render(scene);

   fish_h24->swim(scene,dt);
   fish_h24->update(scene,dt);
   fish_h24->render(scene);

    ///CHROMIS
    fish_h3->swim(scene,dt);
    fish_h3->update(scene,dt);
    fish_h3->render(scene);

    fish_h31->swim(scene,dt);
    fish_h31->update(scene,dt);
    fish_h31->render(scene);

    fish_h32->swim(scene,dt);
    fish_h32->update(scene,dt);
    fish_h32->render(scene);

    fish_h33->swim(scene,dt);
    fish_h33->update(scene,dt);
    fish_h33->render(scene);

    fish_h34->swim(scene,dt);
    fish_h34->update(scene,dt);
    fish_h34->render(scene);
    //fish_l1->update(scene,dt);
    //fish_l1->render(scene);
    //fish_l2->update(scene,dt);
    //fish_l2->render(scene);
    //bubble->update(scene, dt);
    //bubble->render(scene);
    stingray->update(scene,dt);
    stingray->render(scene);
    sponge->update(scene,dt);
    sponge->render(scene);

    sea->update(scene, time);
    sea->render(scene);

    scene.update(dt);
    scene.render();
  }
};

int main() {
  // Initialize our window
  SceneWindow window;

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
