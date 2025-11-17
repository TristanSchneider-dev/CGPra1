#include <GL/glew.h>
#include "coordinatesystem.h"
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glbase/gltool.hpp"
#include "gui/config.h"

#include <QDebug>

const char* simpleVertexShader = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 projection_matrix;
    uniform mat4 modelview_matrix;

    out vec3 vColor;

    void main()
    {
        gl_Position = projection_matrix * modelview_matrix * vec4(aPos, 1.0);
        vColor = aPos * 0.5 + 0.5;
    }
)";

const char* simpleFragmentShader = R"(
    #version 330 core
    out vec4 FragColor;

    in vec3 vColor;
    void main()
    {
        FragColor = vec4(vColor, 1.0);
    }
)";

CoordinateSystem::CoordinateSystem(std::string name) :
    Drawable(name)
{
    qDebug() << "CoordinateSystem constructor called:" << QString::fromStdString(name);
}

void CoordinateSystem::draw(glm::mat4 projection_matrix) const
{
    if (_program == 0)
        return;

    glUseProgram(_program);
    glLineWidth(3.0f);

    GLint projLoc = glGetUniformLocation(_program, "projection_matrix");
    GLint mvLoc = glGetUniformLocation(_program, "modelview_matrix");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

    glBindVertexArray(_vertexArrayObject);
    glDrawArrays(GL_LINES, 0, _verticesCount);
    glBindVertexArray(0);

    glLineWidth(1.0f);
    glUseProgram(0);
}

void CoordinateSystem::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    _modelViewMatrix = modelViewMatrix;
}

void CoordinateSystem::initShader()
{
    qDebug() << "CoordinateSystem::initShader() called.";
    GLuint vs = CG::createCompileShader(GL_VERTEX_SHADER, getVertexShader()); VERIFY(vs);
    GLuint fs = CG::createCompileShader(GL_FRAGMENT_SHADER, getFragmentShader()); VERIFY(fs);

    _program = glCreateProgram();
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);

    glBindAttribLocation(_program, 0, "aPos");

    _program = CG::linkProgram(_program);
    VERIFY(_program);
}

std::string CoordinateSystem::getVertexShader() const
{
    return simpleVertexShader;
}

std::string CoordinateSystem::getFragmentShader() const
{
    return simpleFragmentShader;
}

void CoordinateSystem::createObject()
{
    qDebug() << "CoordinateSystem::createObject() called.";
    std::vector<glm::vec3> vertices;

    vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

    vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

    vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    vertices.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

    _verticesCount = static_cast<GLsizei>(vertices.size());

    glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    glGenBuffers(1, &_vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}