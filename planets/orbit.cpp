#include <GL/glew.h>

#include "orbit.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include <vector>
#include <iostream>

#include "glbase/gltool.hpp"
#include "gui/config.h"

#include <QDebug>

Orbit::Orbit(std::string name, float radius):
    Drawable(name),
    _radius(radius)
{
    qDebug() << "Orbit constructor called for:" << QString::fromStdString(name);
}

void Orbit::draw(glm::mat4 projection_matrix) const
{
    if (!Config::showOrbits)
        return;

    if(_program == 0){
        qDebug() << "Orbit" << QString::fromStdString(_name) << "not initialized. Call init() first.";
        return;
    }

    glUseProgram(_program);

    glBindVertexArray(_vertexArrayObject);

    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

    glUniform3f(glGetUniformLocation(_program, "uColor"), 1.0f, 0.0f, 0.0f);

    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    VERIFY(CG::checkError());
}

void Orbit::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    _modelViewMatrix = modelViewMatrix;
}

std::string Orbit::getVertexShader() const
{
   return Drawable::loadShaderFile(":/shader/simple.vs.glsl");
}

std::string Orbit::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/simple.fs.glsl");
}

void Orbit::createObject()
{
    qDebug() << "Orbit::createObject() called for:" << QString::fromStdString(_name);
    unsigned int segments = _resolutionSegments;
    if (segments < 3) segments = 3;

    float ringWidth = 0.02f;
    float innerRadius = _radius - ringWidth;
    float outerRadius = _radius + ringWidth;


    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < segments; ++i)
    {
        float angle = (float)i / segments * 2.0f * glm::pi<float>();
        float cosA = cos(angle);
        float sinA = sin(angle);
        glm::vec3 innerPos = glm::vec3(cosA * innerRadius, 0.0f, sinA * innerRadius);
        glm::vec3 outerPos = glm::vec3(cosA * outerRadius, 0.0f, sinA * outerRadius);
        positions.push_back(innerPos);
        positions.push_back(outerPos);
        glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
        normals.push_back(normal);
        normals.push_back(normal);
        texCoords.push_back(glm::vec2((float)i / segments, 0.0f));
        texCoords.push_back(glm::vec2((float)i / segments, 1.0f));
        unsigned int i0 = i * 2;
        unsigned int i1 = i * 2 + 1;
        unsigned int i2 = ((i + 1) % segments) * 2;
        unsigned int i3 = ((i + 1) % segments) * 2 + 1;
        indices.push_back(i0);
        indices.push_back(i2);
        indices.push_back(i1);
        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i3);
    }

    _indexCount = static_cast<unsigned int>(indices.size());

    if(_vertexArrayObject == 0)
        glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    if (_positionBuffer == 0)
        glGenBuffers(1, &_positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    if (_normalBuffer == 0)
        glGenBuffers(1, &_normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    if (_texCoordBuffer == 0)
        glGenBuffers(1, &_texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    if (_indexBuffer == 0)
        glGenBuffers(1, &_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    VERIFY(CG::checkError());
}