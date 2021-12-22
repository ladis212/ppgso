#pragma once
#include <memory>

#include <ppgso/ppgso.h>

#include "scene.h"
#include "object.h"


class FishTail final : public Object {
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

    /*!
     * Split the asteroid into multiple pieces and spawn an explosion object.
     *
     * @param scene - Scene to place pieces and explosion into
     * @param explosionPosition - Initial position of the explosion
     * @param explosionScale - Scale of the explosion
     * @param pieces - Island pieces to generate
     */
public:
    /*!
     * Create new asteroid
     */
    FishTail();

    /*!
     * Update asteroid
     * @param scene Scene to interact with
     * @param dt Time delta for animation purposes
     * @return
     */
    bool update(Scene &scene, float time) override;

    /*!
     * Render asteroid
     * @param scene Scene to render in
     */
    void render(Scene &scene) override;

    /*!
     * Custom click event for asteroid
     */

private:
};

