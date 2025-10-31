#include <GL/glew.h>
#include "path.h"

#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>

#include "glbase/gltool.hpp"

#include "gui/config.h"

Path::Path(std::string name):
    Drawable(name),
    _vertexCount(0)
{

}

void Path::draw(glm::mat4 projection_matrix) const
{
    // BEHOBEN: Die Variable Config::orbits existiert nicht.
    // if (!Config::orbits)
    //     return;

    if(_program == 0 || _vertexCount == 0){
        // Kein Fehler, da Pfade erst berechnet werden müssen
        return;
    }

    glUseProgram(_program);
    glBindVertexArray(_vertexArrayObject);

    // Setze Matrizen
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

    // Setze Pfad-Farbe (z.B. weiß)
    glUniform4f(glGetUniformLocation(_program, "uColor"), 1.0f, 1.0f, 1.0f, 1.0f);

    // Zeichne als Linienzug
    glDrawArrays(GL_LINE_STRIP, 0, _vertexCount);

    glBindVertexArray(0);
    VERIFY(CG::checkError());
}

void Path::createObject()
{
    // Diese Methode wird aufgerufen, NACHDEM addPosition() alle Punkte hinzugefügt hat

    _vertexCount = static_cast<unsigned int>(_positions.size());
    if (_vertexCount == 0)
        return; // Nichts zu erstellen

    if(_vertexArrayObject == 0)
        glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    // VBO für Positionen
    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, _positions.size() * sizeof(glm::vec3), _positions.data(), GL_STATIC_DRAW);

    // WICHTIG: Nur location 0 aktivieren!
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    VERIFY(CG::checkError());
}

void Path::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    // Die Pfad-Punkte sind bereits in Welt-Koordinaten (berechnet in Planet::addPathPoint)
    // Daher ist die ModelView-Matrix für den Pfad selbst die Identitätsmatrix.
    _modelViewMatrix = glm::mat4(1.0f);
}

void Path::addPosition(glm::vec3 position)
{
    // Speichere den berechneten Punkt
    _positions.push_back(position);
}

std::string Path::getVertexShader() const
{
    // Verwendet den NEUEN Path-Shader
    return Drawable::loadShaderFile(":/shader/path.vs.glsl");
}

std::string Path::getFragmentShader() const
{
    // Verwendet den NEUEN Path-Shader
    return Drawable::loadShaderFile(":/shader/path.fs.glsl");
}