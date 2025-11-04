#include <GL/glew.h>

#include "orbit.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp> // Für glm::pi()

#include <vector>
#include <iostream> // Behoben: Fehlender Include für cerr und endl

#include "glbase/gltool.hpp"

#include "gui/config.h"

Orbit::Orbit(std::string name, float radius):
    Drawable(name),
    _radius(radius)
{

}

void Orbit::draw(glm::mat4 projection_matrix) const
{
    // Behoben: Die Variable Config::orbits schien nicht zu existieren.
    // Falls Sie die Orbits global an/ausschalten möchten,
    // müssen Sie eine statische bool-Variable 'orbits' in der Config-Klasse hinzufügen.
    if (!Config::showOrbits)
        return;

    if(_program == 0){
        std::cerr << "Orbit" << _name << "not initialized. Call init() first." << std::endl;
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

void Orbit::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    // Der Orbit selbst bewegt oder dreht sich nicht,
    // er übernimmt einfach die ModelView-Matrix seines "Zentrums" (z.B. der Sonne)
    _modelViewMatrix = modelViewMatrix;
}

std::string Orbit::getVertexShader() const
{
   // Orbits sind oft nur einfach gefärbt und nicht beleuchtet.
   // "cube.vs.glsl" ist hier wahrscheinlich falsch.
   // Ich verwende "simple.vs.glsl" als Platzhalter.
   return Drawable::loadShaderFile(":/shader/simple.vs.glsl");
}

std::string Orbit::getFragmentShader() const
{
    // Passend zum simple.vs.glsl
    return Drawable::loadShaderFile(":/shader/simple.fs.glsl");
}

void Orbit::createObject()
{
    // Parametrisierung für den Ring (Annulus)
    unsigned int segments = 100; // Auflösung des Rings

    // Wir definieren eine "Dicke" für den Ring, damit er sichtbar ist.
    // Hier z.B. 1% des Radius auf beiden Seiten.
    float ringWidth = _radius * 0.01f;
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

        // Positionen (flach in der XZ-Ebene)
        glm::vec3 innerPos = glm::vec3(cosA * innerRadius, 0.0f, sinA * innerRadius);
        glm::vec3 outerPos = glm::vec3(cosA * outerRadius, 0.0f, sinA * outerRadius);

        positions.push_back(innerPos);
        positions.push_back(outerPos);

        // Normale (zeigt für einen flachen Ring immer nach oben)
        glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
        normals.push_back(normal);
        normals.push_back(normal);

        // Texturkoordinaten
        texCoords.push_back(glm::vec2((float)i / segments, 0.0f)); // v=0 (innen)
        texCoords.push_back(glm::vec2((float)i / segments, 1.0f)); // v=1 (außen)

        // Indizes (bildet Quads aus Dreiecken)
        unsigned int i0 = i * 2;
        unsigned int i1 = i * 2 + 1;
        // % (segments * 2) sorgt für den Ringschluss (letztes Segment -> erstes Segment)
        unsigned int i2 = ((i + 1) % segments) * 2;
        unsigned int i3 = ((i + 1) % segments) * 2 + 1;

        // Erstes Dreieck
        indices.push_back(i0);
        indices.push_back(i2);
        indices.push_back(i1);

        // Zweites Dreieck
        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i3);
    }

    _indexCount = static_cast<unsigned int>(indices.size());

    // Set up a vertex array object for the geometry
    if(_vertexArrayObject == 0)
        glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    // Position-Buffer (loc 0)
    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Normalen-Buffer (loc 1)
    GLuint normal_buffer;
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Texturkoordinaten-Buffer (loc 2)
    GLuint texcoord_buffer;
    glGenBuffers(1, &texcoord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, texcoord_buffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Index-Buffer
    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // unbind vertex array object
    glBindVertexArray(0);

    // check for errors
    VERIFY(CG::checkError());
}