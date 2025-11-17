#include "planets/ring.h"

#include <GL/glew.h>
#include <vector>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glbase/gltool.hpp"
#include "gui/config.h"
#include "planets/sun.h"

#include <QDebug>

Ring::Ring(std::string name,
           float innerRadius,
           float outerRadius,
           std::string textureLocation,
           float axialTilt)
    : Drawable(name),
      _innerRadius(innerRadius),
      _outerRadius(outerRadius),
      _axialTilt(axialTilt),
      _textureLocation(textureLocation),
      _textureID(0),
      _indexCount(0)
{
    qDebug() << "Ring constructor called for:" << QString::fromStdString(_name);
}

void Ring::init()
{
    qDebug() << "Ring::init() called for:" << QString::fromStdString(_name);
    Drawable::init();

    if (!_textureLocation.empty())
    {
        _textureID = loadTexture(_textureLocation);
        if (_textureID == 0)
        {
            qDebug() << "Could not load texture for" << QString::fromStdString(_name) << "from" << QString::fromStdString(_textureLocation);
        }
    }
}

void Ring::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    _modelViewMatrix = glm::rotate(modelViewMatrix, glm::radians(_axialTilt), glm::vec3(1.0f, 0.0f, 0.0f));
}

void Ring::draw(glm::mat4 projection_matrix) const
{
    if (_program == 0)
    {
        qDebug() << "Ring" << QString::fromStdString(_name) << "not initialized. Call init() first.";
        return;
    }

    glUseProgram(_program);
    glBindVertexArray(_vertexArrayObject);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glUniform1i(glGetUniformLocation(_program, "uTextureSampler"), 0);

    glm::vec3 lightPosView = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    if (Config::sunLight && _sun)
    {
        lightPosView = _sun->getPosition();
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    glUniform3fv(glGetUniformLocation(_program, "uLightPosView"), 1, glm::value_ptr(lightPosView));
    glUniform3fv(glGetUniformLocation(_program, "uLightColor"), 1, glm::value_ptr(lightColor));

    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    VERIFY(CG::checkError());
}

void Ring::createObject()
{
    qDebug() << "Ring::createObject() called for:" << QString::fromStdString(_name);
    unsigned int segments = _resolutionSegments;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i <= segments; ++i)
    {
        float u = (float)i / segments;
        float angle = u * glm::radians(360.0f);
        float cosA = cos(angle);
        float sinA = sin(angle);

        positions.push_back(glm::vec3(_innerRadius * cosA, 0.0f, _innerRadius * sinA));
        normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        texCoords.push_back(glm::vec2(0.0f, u));

        positions.push_back(glm::vec3(_outerRadius * cosA, 0.0f, _outerRadius * sinA));
        normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        texCoords.push_back(glm::vec2(1.0f, u));
    }

    for (unsigned int i = 0; i < segments; ++i)
    {
        unsigned int v1 = i * 2;
        unsigned int v2 = v1 + 1;
        unsigned int v3 = (i + 1) * 2;
        unsigned int v4 = v3 + 1;

        indices.push_back(v1);
        indices.push_back(v3);
        indices.push_back(v2);

        indices.push_back(v2);
        indices.push_back(v3);
        indices.push_back(v4);
    }

    _indexCount = static_cast<unsigned int>(indices.size());

    if (_vertexArrayObject == 0)
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

void Ring::setLights(std::shared_ptr<Sun> sun, std::shared_ptr<Cone> laser)
{
    qDebug() << "Ring::setLights() called for:" << QString::fromStdString(_name);
    _sun = sun;
    _laser = laser;
}

std::string Ring::getVertexShader() const
{
    return Drawable::loadShaderFile(":/shader/phong.vs.glsl");
}

std::string Ring::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/ring.fs.glsl");
}