#pragma once
#include <memory>

#include <ppgso/ppgso.h>

#include "scene.h"
#include "object.h"


class fish final : public Object {
private:
    // Static resources (Shared between instances)
    std::unique_ptr<ppgso::Mesh> mesh;
    std::unique_ptr<ppgso::Shader> shader;
    std::unique_ptr<ppgso::Texture> texture;
    int varination = glm::linearRand(0, 3);
    // Age of the object in seconds
    float age{0.0f};

    // Speed and rotational momentum
public:

    glm::vec3 speed;
    glm::vec3 rotMomentum;

    /*!
     * Split the asteroid into multiple pieces and spawn an explosion object.
     *
     * @param scene - Scene to place pieces and explosion into
     * @param explosionPosition - Initial position of the explosion
     * @param explosionScale - Scale of the explosion
     * @param pieces - Island pieces to generate
     */
public:

    fish(const std::basic_string<char> &mesh_path, const std::basic_string<char> &texture_path,  const std::string &vert, const std::string &frag);

    bool update(Scene &scene, float dt) override;
    void collide (Scene &scene, float dt);
    void swim (Scene &scene, float dt);
    void render(Scene &scene) override;


    /*!
     * Custom click event for asteroid
     */
    void onClick(Scene &scene) override;
    float time = 0.0f;
private:
};

