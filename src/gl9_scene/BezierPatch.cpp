//
// Created by ladis on 23. 11. 2021.
//

#include "BezierPatch.h"

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


#include <shaders/sea_vert_glsl.h>
#include <shaders/sea_frag_glsl.h>

#define PATCH_SIZE 300

std::unique_ptr<ppgso::Shader> BezierPatch::shader;
std::unique_ptr<ppgso::Texture> BezierPatch::texture;
GLuint vao, vbo, tbo, ibo, n_vbo;



    BezierPatch::BezierPatch() {
        // Generate Bezier patch points and incidences
        if (!shader) shader = std::make_unique<ppgso::Shader>(sea_vert_glsl, sea_frag_glsl);
        if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("sea.bmp"));

        float u,v, real_x, real_z;
        for (unsigned int i = 0; i < PATCH_SIZE; i++) {
            for (unsigned int j = 0; j < PATCH_SIZE; j++) {
                // TODO: Compute points on the bezier patch
                // HINT: Compute u, v coordinates
                u = float(i) / float(PATCH_SIZE - 1);
                v = float(j) / float(PATCH_SIZE - 1);

                real_x = i * 0.6 - PATCH_SIZE * 0.3;
                real_z = j * 0.6 - PATCH_SIZE * 0.3;


                vertices.emplace_back(real_x, 0, real_z);
                //printf("%f %f\n", u, v);
                texCoords.emplace_back(glm::vec2{u * 6, 1 - v * 6}); // *6 - opakujuca textura
            }
        }
        // Generate indices
        for (unsigned int i = 1; i < PATCH_SIZE; i++) {
            for (unsigned int j = 1; j < PATCH_SIZE; j++) {
                //Compute indices for triangle 1
                mesh.push_back(i * PATCH_SIZE + j);
                mesh.push_back((i - 1) * PATCH_SIZE + (j - 1));
                mesh.push_back((i - 1) * PATCH_SIZE + j);

                //Compute indices for triangle 2
                mesh.push_back(i * PATCH_SIZE + j);
                mesh.push_back(i * PATCH_SIZE + (j - 1));
                mesh.push_back((i - 1) * PATCH_SIZE + (j - 1));

            }
        }

        // Copy data to OpenGL
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Copy positions to gpu
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);\

        // Set vertex program inputs
        auto position_attrib = shader->getAttribLocation("Position");
        glEnableVertexAttribArray(position_attrib);
        glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Copy texture positions to gpu
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_DYNAMIC_DRAW);

        // Set vertex program inputs
        auto texCoord_attrib = shader->getAttribLocation("TexCoord");
        glEnableVertexAttribArray(texCoord_attrib);
        glVertexAttribPointer(texCoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

        // Normals
        glGenBuffers(1, &n_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, n_vbo);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_DYNAMIC_DRAW);

        auto normal_attrib = shader->getAttribLocation("Normal");
        glEnableVertexAttribArray(normal_attrib);
        glVertexAttribPointer(normal_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(0);

        // Copy indices to gpu
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.size() * sizeof(GLuint), mesh.data(), GL_DYNAMIC_DRAW);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, wiremesh.size(), wiremesh.data(), GL_STATIC_DRAW);



    };

    // Clean up
    BezierPatch::~BezierPatch() {
        // Delete data from OpenGL
        glDeleteBuffers(1, &ibo);
        glDeleteBuffers(1, &tbo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    // Set the object transformation matrix
    bool BezierPatch::update(Scene &scene, float time) {
        for (unsigned int i = 0; i < PATCH_SIZE; i++) {
            for (unsigned int j = 0; j < PATCH_SIZE; j++) {
                float real_x = i * 0.6 - PATCH_SIZE * 0.3;
                float real_z = j * 0.6 - PATCH_SIZE * 0.3;

                vertices[i*PATCH_SIZE + j].y = sin(time)/3 + sin(float(real_x)/3 + time)/2 + cos(float(real_z)/3 + time)/2 + sin(float(real_x)/9 + time)/2;
            }
        }

        return true;
    }

void BezierPatch::render(Scene &scene) {

    shader->use();

    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // Set model position
    shader->setUniform("ModelMatrix", modelMatrix);

    // Bind texture
    shader->setUniform("Texture", *texture);
    shader->setUniform("Transparency", 1);

    shader->setUniform("Time", glfwGetTime());
    shader->setUniform("EyePos", scene.camera->position);


    glBindVertexArray(vao);
    //Use correct rendering mode to draw the result
    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
    glDrawElements(GL_TRIANGLES, mesh.size(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
