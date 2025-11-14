#include <GL/glew.h>

#include "cone.h"

#include <vector>
#include <iostream> // für std::cerr/endl
#include <cmath> // für atan
#include <glm/gtc/constants.hpp> // für glm::pi()

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glbase/gltool.hpp"

#include "gui/config.h"

Cone::Cone(std::string name, float distance):
    Drawable(name),
    _distance(distance), _angle(.0f)
{

}



void Cone::draw(glm::mat4 projection_matrix) const
{
    // Behoben: Die Abfrage auf Config::laser wurde entfernt.
    /*
    if (!Config::laser)
        return;
    */

    if(_program == 0){
        std::cerr << "Cone" << _name << "not initialized. Call init() first." << std::endl;
        return;
    }

    // Load program
    glUseProgram(_program);

    // bin vertex array object
    glBindVertexArray(_vertexArrayObject);

    // set parameter
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

    // call draw
    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);

    // unbin vertex array object
    glBindVertexArray(0);

    // check for errors
    VERIFY(CG::checkError());
}

void Cone::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    // Die modelViewMatrix ist die des Elternelements (Death Star Kern).
    // Wir verschieben den Kegel an die Oberfläche (entlang +Y)
    _modelViewMatrix = glm::translate(modelViewMatrix, glm::vec3(0.0f, _distance, 0.0f));

    // Speichern der globalen Position (Apex des Kegels)
    _position = glm::vec3(_modelViewMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // Speichern der globalen Richtung (die Y-Achse des Kegels)
    _direction = glm::normalize(glm::vec3(_modelViewMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
}

std::string Cone::getVertexShader() const
{
    // Annahme: simple shader (wie beim Orbit)
    return Drawable::loadShaderFile(":/shader/simple.vs.glsl");
}

std::string Cone::getFragmentShader() const
{
    // Annahme: simple shader (wie beim Orbit)
    return Drawable::loadShaderFile(":/shader/simple.fs.glsl");
}

void Cone::createObject()
{
    float height = 10.0f;
    float baseRadius = 0.5f;
    unsigned int segments = _resolutionSegments;

    _angle = glm::degrees(atan(baseRadius / height));

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    // ... (Die Logik zum Füllen der Vektoren bleibt exakt gleich) ...
    unsigned int apexIndex = 0;
    positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    texCoords.push_back(glm::vec2(0.5f, 0.5f));
    unsigned int baseCenterIndex = 1;
    positions.push_back(glm::vec3(0.0f, height, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    texCoords.push_back(glm::vec2(0.5f, 0.5f));
    unsigned int currentIndex = 2;
    for(unsigned int i = 0; i <= segments; ++i)
    {
        float angle = (float)i / segments * 2.0f * glm::pi<float>();
        float x = cos(angle) * baseRadius;
        float z = sin(angle) * baseRadius;
        positions.push_back(glm::vec3(x, height, z));
        normals.push_back(glm::normalize(glm::vec3(x, baseRadius, z)));
        texCoords.push_back(glm::vec2((float)i / segments, 1.0f));
        unsigned int sideIndex = currentIndex++;
        positions.push_back(glm::vec3(x, height, z));
        normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        texCoords.push_back(glm::vec2(cos(angle) * 0.5f + 0.5f, sin(angle) * 0.5f + 0.5f));
        unsigned int baseIndex = currentIndex++;
    }
    for(unsigned int i = 0; i < segments; ++i)
    {
        unsigned int i0_side = 2 + i * 2;
        unsigned int i1_side = 2 + (i + 1) * 2;
        unsigned int i0_base = 2 + i * 2 + 1;
        unsigned int i1_base = 2 + (i + 1) * 2 + 1;
        indices.push_back(apexIndex);
        indices.push_back(i1_side);
        indices.push_back(i0_side);
        indices.push_back(baseCenterIndex);
        indices.push_back(i0_base);
        indices.push_back(i1_base);
    }

    _indexCount = static_cast<unsigned int>(indices.size());

    // VAO
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
    // --- ENDE NEUE BUFFER-VERWALTUNG ---

    glBindVertexArray(0);
    VERIFY(CG::checkError());
}

glm::vec3 Cone::getDirection() const
{
    return _direction;
}

glm::vec3 Cone::getPosition() const
{
    return _position;
}

float Cone::getAngle() const
{
    // _angle wird jetzt in createObject() berechnet
    return _angle;
}