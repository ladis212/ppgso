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

#include "BezierPatch.h"

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

const unsigned int HEIGHT = 720;
const unsigned int WIDTH = 720;

/*!
 * Custom windows for our simple game
 */
class SceneWindow : public ppgso::Window {
private:
  Scene scene;
  bool animate = true;
  std::unique_ptr<Skybox> skybox;
  std::unique_ptr<Island> island;
  std::unique_ptr<GenericObject> dolphin;
  std::unique_ptr<Bubble> bubble;
  std::unique_ptr<ppgso::Texture> skybox_alt_texture;

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
      island->position = {0, -20, 0};
      //bubble = std::make_unique<Bubble>();
      //bubble->scale = {10, 10, 10};
      //bubble->rotation = {0, 0, M_PI};
      //scene.objects.push_back(move(island));
      //skybox->position = {0, 10, 0};
      auto generator = std::make_unique<Generator>();
      generator->position.y = -10.0f;
      scene.objects.push_back(move(generator));
      //dolphin = std::make_unique<Dolphin>();
      dolphin = std::make_unique<GenericObject>("dolphin\\10014_dolphin_v2_max2011_it2.obj", "dolphin\\10014_dolphin_v1_Diffuse.bmp", diffuse_vert_glsl, diffuse_frag_glsl);
      dolphin->scale = {.05, .05, .05};;
      //skybox->update(scene, 0);
      // Add generator to scene
      //auto generator = std::make_unique<Generator>();
      //generator->position.y = 10.0f;
      //scene.objects.push_back(move(generator));

      //// Add player to the scene
      //auto player = std::make_unique<Player>();
      //player->position.y = -6;
      //scene.objects.push_back(move(player));
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
        if(action == GLFW_PRESS) {scene.camera->position.y -= 1; scene.camera->target.y -= 1;}
        if(action == GLFW_RELEASE);

    }

    if (key == GLFW_KEY_E) {
        if(action == GLFW_PRESS) {scene.camera->position.y += 1; scene.camera->target.y += 1;}
        if(action == GLFW_RELEASE);

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
   * @param mods
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
    scene.camera->position += (directions * direction) * speed;
    //move(skybox);

    time = (float) glfwGetTime();

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
    //bubble->update(scene, dt);
    //bubble->render(scene);

    //pohyb delfina:
    //X := originX + cos(angle)*radius;
     //Y := originY + sin(angle)*radius;


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
