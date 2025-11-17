#include <GL/glew.h>
#include "path.h"

#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>

#include "glbase/gltool.hpp"

#include "gui/config.h"

#include <QDebug>

Path::Path(std::string name):
    Drawable(name),
    _vertexCount(0)
{
    qDebug() << "Path constructor called for:" << QString::fromStdString(name);
}

void Path::draw(glm::mat4 projection_matrix) const
{
    if(_program == 0 || _vertexCount == 0){
        return;
    }

    glUseProgram(_program);
    glBindVertexArray(_vertexArrayObject);

    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

    glUniform4f(glGetUniformLocation(_program, "uColor"), 1.0f, 1.0f, 1.0f, 1.0f);

    glDrawArrays(GL_LINE_STRIP, 0, _vertexCount);

    glBindVertexArray(0);
    VERIFY(CG::checkError());
}

void Path::createObject()
{
    qDebug() << "Path::createObject() called for:" << QString::fromStdString(_name);

    _vertexCount = static_cast<unsigned int>(_positions.size());
    if (_vertexCount == 0)
    {
        qDebug() << "Path::createObject() - No vertices to create.";
        return;
    }

    if(_vertexArrayObject == 0)
        glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, _positions.size() * sizeof(glm::vec3), _positions.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    VERIFY(CG::checkError());
}

void Path::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    _modelViewMatrix = glm::mat4(1.0f);
}

void Path::addPosition(glm::vec3 position)
{
    _positions.push_back(position);
}

std::string Path::getVertexShader() const
{
    return Drawable::loadShaderFile(":/shader/path.vs.glsl");
}

std::string Path::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/path.fs.glsl");
}