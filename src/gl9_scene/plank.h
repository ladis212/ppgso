#pragma once
#include <memory>

#include <ppgso/ppgso.h>

#include "scene.h"
#include "object.h"


class Plank final : public Object {
private:
    // Static resources (Shared between instances)
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;

   // float age{5000000000000000000000000000000.0f}; it is really old ok?

    // Speed and rotational momentum
public:

    glm::vec3 speed;
    glm::vec3 rotMomentum;

public:

    Plank();

    bool update(Scene &scene, float dt) override;

    void shatter(Scene &scene, int pieces);

    void render(Scene &scene) override;

    void onClick(Scene &scene) override;

private:
};

