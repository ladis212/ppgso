#pragma once
#include <memory>

#include <ppgso/ppgso.h>

#include "scene.h"
#include "object.h"

class Sponge final : public Object {
private:
    // Static resources (Shared between instances)
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;

    // Age of the object in seconds
    float age{0.0f};

    // Speed and rotational momentum
public:

    glm::vec3 speed;
    glm::vec3 rotMomentum;

public:
    //constructor
    Sponge();

    //Update, Render, etc
    bool update(Scene &scene, float dt) override;
    void breathe(Scene &scene);
    void render(Scene &scene) override;

    void onClick(Scene &scene) override;

private:
};

