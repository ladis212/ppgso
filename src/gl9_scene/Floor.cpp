//
// Created by ladis on 23. 11. 2021.
//

#include "Floor.h"
#include "fish.h"

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


#include <shaders/underwater_vert_glsl.h>
#include <shaders/underwater_frag_glsl.h>

#define PATCH_SIZE 300

std::unique_ptr<ppgso::Shader> Floor::shader;
std::unique_ptr<ppgso::Texture> Floor::texture;
GLuint floor_vao, floor_vbo, floor_tbo, floor_ibo;

float Floor::getDisplacement(float x, float z){
    return std::sin(x/5)/2 + std::cos(z/5)/2; // vyska v Y osi
}

Floor::Floor() {
    float displace = 0;
    // Generate Bezier patch points and incidences
    if (!shader) shader = std::make_unique<ppgso::Shader>(underwater_vert_glsl, underwater_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("sand.bmp"));

    float u,v;
    float real_x, real_z;
    for (unsigned int i = 0; i < PATCH_SIZE; i++) {
        for (unsigned int j = 0; j < PATCH_SIZE; j++) {
            // TODO: Compute points on the bezier patch
            // HINT: Compute u, v coordinates
            u = float(i) / float(PATCH_SIZE - 1);
            v = float(j) / float(PATCH_SIZE - 1);

            real_x = u * 180 - 90;
            real_z = v * 180 - 90;

            displace = getDisplacement(real_x, real_z);

            vertices.emplace_back(real_x, displace, real_z);
            //printf("%f %f\n", u, v);
            texCoords.emplace_back(glm::vec2{u, 1 - v});
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
    glGenVertexArrays(1, &floor_vao);
    glBindVertexArray(floor_vao);

    // Copy positions to gpu
    glGenBuffers(1, &floor_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, floor_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);\

    // Set vertex program inputs
    auto position_attrib = shader->getAttribLocation("Position");
    glEnableVertexAttribArray(position_attrib);
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Copy texture positions to gpu
    glGenBuffers(1, &floor_tbo);
    glBindBuffer(GL_ARRAY_BUFFER, floor_tbo);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_DYNAMIC_DRAW);

    // Set vertex program inputs
    auto texCoord_attrib = shader->getAttribLocation("TexCoord");
    glEnableVertexAttribArray(texCoord_attrib);
    glVertexAttribPointer(texCoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Copy indices to gpu
    glGenBuffers(1, &floor_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floor_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.size() * sizeof(GLuint), mesh.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, wiremesh.size(), wiremesh.data(), GL_STATIC_DRAW);



};

// Clean up
Floor::~Floor() {
    // Delete data from OpenGL
    glDeleteBuffers(1, &floor_ibo);
    glDeleteBuffers(1, &floor_tbo);
    glDeleteBuffers(1, &floor_vbo);
    glDeleteVertexArrays(1, &floor_vao);
}

// Set the object transformation matrix
bool Floor::update(Scene &scene, float time) {

    modelMatrix =
            glm::translate(glm::mat4(1.0f), position)
            * glm::orientate4(rotation)
            * glm::scale(glm::mat4(1.0f), scale);

    return true;
}

bool Floor::render(Scene &scene) {

    shader->use();

    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // Set model position
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("HasNormals", -1.0f);

    // Bind texture
    shader->setUniform("Texture", *texture);
    shader->setUniform("Transparency", 1);

    shader->setUniform("Time", glfwGetTime());
    shader->setUniform("cameraView", scene.camera->position);

    glUniform1i(glGetUniformLocation(shader->getProgram(), "castShadows"), 1);
    for(int i = 0; i < 30; i++){
        glUniform3fv(glGetUniformLocation(shader->getProgram(), ("Shadows[" + std::to_string(i) + ']').c_str()), 1, glm::value_ptr(fish::shadowPositions[i]));
    }


    glBindVertexArray(floor_vao);
    //Use correct rendering mode to draw the result
    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floor_ibo);
    glBindBuffer(GL_ARRAY_BUFFER, floor_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
    glDrawElements(GL_TRIANGLES, mesh.size(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return false;
}
