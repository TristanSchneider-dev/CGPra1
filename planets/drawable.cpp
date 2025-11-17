#include "planets/drawable.h"

#include <QFile>
#include <QTextStream>
#include <QImage>
#include <QGLWidget>
#include <QDebug>

#include <iostream>
#include <vector>

#include "glbase/gltool.hpp"

Drawable::Drawable(std::string name):
    _name(name),
    _program(0),
    _modelViewMatrix(glm::mat4(1.0f)),
    _resolutionSegments(60),
    _vertexArrayObject(0),
    _positionBuffer(0),
    _normalBuffer(0),
    _texCoordBuffer(0),
    _indexBuffer(0)
{
    qDebug() << "Drawable constructor called for:" << QString::fromStdString(_name);
}

void Drawable::init()
{
    qDebug() << "Drawable::init() called for:" << QString::fromStdString(_name);
    initShader();
    createObject();
}

void Drawable::recreate()
{
    qDebug() << "Drawable::recreate() called for:" << QString::fromStdString(_name);
    createObject();
}

void Drawable::setResolution(unsigned int segments)
{
    qDebug() << "Drawable::setResolution() called for:" << QString::fromStdString(_name) << "with segments:" << segments;
    if (segments < 3)
        _resolutionSegments = 3;
    else
        _resolutionSegments = segments;

    recreate();
}

void Drawable::initShader()
{
    qDebug() << "Drawable::initShader() called for:" << QString::fromStdString(_name);
    _program = glCreateProgram();

    std::string vs_string = getVertexShader();
    std::string fs_string = getFragmentShader();
    const char* vs_data = vs_string.c_str();
    const char* fs_data = fs_string.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_data, NULL);
    glCompileShader(vs);

    GLint vs_status;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &vs_status);
    if (vs_status == GL_FALSE) {
        GLint logLen;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetShaderInfoLog(vs, logLen, NULL, log.data());
        qDebug() << "Vertex Shader Compile Error (" << QString::fromStdString(_name) << "): " << log.data();
    }
    glAttachShader(_program, vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_data, NULL);
    glCompileShader(fs);

    GLint fs_status;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &fs_status);
    if (fs_status == GL_FALSE) {
        GLint logLen;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetShaderInfoLog(fs, logLen, NULL, log.data());
        qDebug() << "Fragment Shader Compile Error (" << QString::fromStdString(_name) << "): " << log.data();
    }
    glAttachShader(_program, fs);

    glLinkProgram(_program);

    GLint link_status;
    glGetProgramiv(_program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        GLint logLen;
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetProgramInfoLog(_program, logLen, NULL, log.data());
        qDebug() << "Shader Program Link Error (" << QString::fromStdString(_name) << "): " << log.data();
    }
}

std::string Drawable::loadShaderFile(std::string path) const
{
    QFile f(QString::fromStdString(path));
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Could not load shader file:" << QString::fromStdString(path);
        return "";
    }
    QTextStream in(&f);
    return in.readAll().toStdString();
}

GLuint Drawable::loadTexture(std::string path)
{
    QImage tex;
    tex.load(QString::fromStdString(path));
    tex = QGLWidget::convertToGLFormat(tex);

    if(tex.isNull()){
        qDebug() << "Could not load texture file:" << QString::fromStdString(path);
        return 0;
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}