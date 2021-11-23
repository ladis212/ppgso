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

#include "camera.h"
#include "scene.h"
#include "generator.h"
#include "player.h"
#include "space.h"
#include "skybox.h"

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

class BezierPatchSea {
private:
    // 3D vectors define points/vertices of the shape
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> wirevertices;

    // Texture coordinates
    std::vector<glm::vec2> texCoords;

    // Define our face using indexes to 3 vertices
    //struct face {
    //  GLuint v0, v1, v2;
    //};

    // Define our mesh as collection of faces
    std::vector<GLuint> mesh;
    std::vector<GLuint> wiremesh;

    // These will hold the data and object buffers
    GLuint vao, vbo, tbo, ibo;
    glm::mat4 modelMatrix{1.0f};

    glm::vec3 bezierPoint(const glm::vec3 controlPoints[4], float t) {
        // TODO: Compute 3D point on bezier curve
        glm::vec3 first_01 = glm::lerp(controlPoints[0], controlPoints[1], t);
        glm::vec3 first_12 = glm::lerp(controlPoints[1], controlPoints[2], t);
        glm::vec3 first_23 = glm::lerp(controlPoints[2], controlPoints[3], t);

        glm::vec3 second_01 = glm::lerp(first_01, first_12, t);
        glm::vec3 second_12 = glm::lerp(first_12, first_23, t);

        glm::vec3 third_01 = glm::lerp(second_01, second_12, t);
        return third_01;
    }

    ppgso::Shader program{texture_vert_glsl, texture_frag_glsl};
    ppgso::Texture texture{ppgso::image::loadBMP("lena.bmp")};
public:
    // Public attributes that define position, color ..
    glm::vec3 position{0, 0, 0};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 scale{1, 1, 1};

    // Initialize object data buffers
    BezierPatch(const glm::vec3 controlPoints[4][4]) {
        // Generate Bezier patch points and incidences
        float u, v;
        glm::vec3 uPoints[4];
        glm::vec3 vPoint;

        for (int i = 0; i < 16; i++)
            wirevertices.emplace_back(controlPoints[i / 4][i % 4]);


        // Generate wire indices
        for (unsigned int i = 1; i < 4; i++) {
            for (unsigned int j = 1; j < 4; j++) {
                //Compute indices for triangle 1
                wiremesh.push_back(i * 4 + j);
                wiremesh.push_back((i - 1) * 4 + (j - 1));
                wiremesh.push_back((i - 1) * 4 + j);

                //Compute indices for triangle 2
                wiremesh.push_back(i * 4 + j);
                wiremesh.push_back((i - 1) * 4 + (j - 1));
                wiremesh.push_back(i * 4 + (j - 1));
            }
        }

        unsigned int PATCH_SIZE = 10;
        for (unsigned int i = 0; i < PATCH_SIZE; i++) {
            for (unsigned int j = 0; j < PATCH_SIZE; j++) {
                // TODO: Compute points on the bezier patch
                // HINT: Compute u, v coordinates
                u = float(i) / float(PATCH_SIZE - 1);
                v = float(j) / float(PATCH_SIZE - 1);

                uPoints[0] = bezierPoint(controlPoints[0], u);
                uPoints[1] = bezierPoint(controlPoints[1], u);
                uPoints[2] = bezierPoint(controlPoints[2], u);
                uPoints[3] = bezierPoint(controlPoints[3], u);
                vPoint = bezierPoint(uPoints, v);

                vertices.push_back(vPoint);
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
                mesh.push_back((i - 1) * PATCH_SIZE + (j - 1));
                mesh.push_back(i * PATCH_SIZE + (j - 1));
            }
        }

        // Copy data to OpenGL
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Copy positions to gpu
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, wirevertices.size() * sizeof(glm::vec3), wirevertices.data(), GL_STATIC_DRAW);

        // Set vertex program inputs
        auto position_attrib = program.getAttribLocation("Position");
        glEnableVertexAttribArray(position_attrib);
        glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Copy texture positions to gpu
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);

        // Set vertex program inputs
        auto texCoord_attrib = program.getAttribLocation("TexCoord");
        glEnableVertexAttribArray(texCoord_attrib);
        glVertexAttribPointer(texCoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

        // Copy indices to gpu
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.size() * sizeof(GLuint), mesh.data(), GL_STATIC_DRAW);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, wiremesh.size(), wiremesh.data(), GL_STATIC_DRAW);

    };

    // Clean up
    ~BezierPatch() {
        // Delete data from OpenGL
        glDeleteBuffers(1, &ibo);
        glDeleteBuffers(1, &tbo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    // Set the object transformation matrix
    void update() {
        // TODO: Compute transformation by scaling, rotating and then translating the shape
        // modelMatrix = ??
        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale[0], scale[1], scale[2]));

        modelMatrix = glm::rotate(modelMatrix, rotation[0], glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, rotation[1], glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, rotation[2], glm::vec3(0, 0, 1));

        modelMatrix = glm::translate(modelMatrix, glm::vec3(position[0], position[1], position[2]));

    }
};