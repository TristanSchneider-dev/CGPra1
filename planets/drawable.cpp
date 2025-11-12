#include "planets/drawable.h"

#include <QFile>
#include <QTextStream>
#include <QImage>
#include <QGLWidget> // Für convertToGLFormat

#include <iostream>
#include <vector>

#include "glbase/gltool.hpp" // Für VERIFY

Drawable::Drawable(std::string name):
    _name(name),
    _program(0),
    _vertexArrayObject(0),
    _modelViewMatrix(glm::mat4(1.0f)) // Initialisieren
{
}

void Drawable::init()
{
    // Lade Shader und erstelle das Objekt
    initShader();
    createObject();
}

void Drawable::recreate()
{
    // Erstelle das Objekt neu (z.B. wenn sich Parameter ändern)
    createObject();
}

// --- KOMPLETT NEUE initShader() FUNKTION ---
void Drawable::initShader()
{
    _program = glCreateProgram();

    std::string vs_string = getVertexShader();
    std::string fs_string = getFragmentShader();
    const char* vs_data = vs_string.c_str();
    const char* fs_data = fs_string.c_str();

    // --- Vertex Shader Kompilierung ---
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_data, NULL);
    glCompileShader(vs);

    // Fehlerprüfung
    GLint vs_status;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &vs_status);
    if (vs_status == GL_FALSE) {
        GLint logLen;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetShaderInfoLog(vs, logLen, NULL, log.data());
        std::cerr << "Vertex Shader Compile Error (" << _name << "): " << log.data() << std::endl;
    }
    glAttachShader(_program, vs);

    // --- Fragment Shader Kompilierung ---
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_data, NULL);
    glCompileShader(fs);

    // Fehlerprüfung
    GLint fs_status;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &fs_status);
    if (fs_status == GL_FALSE) {
        GLint logLen;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetShaderInfoLog(fs, logLen, NULL, log.data());
        std::cerr << "Fragment Shader Compile Error (" << _name << "): " << log.data() << std::endl;
    }
    glAttachShader(_program, fs);

    // --- Shader-Programm Linken ---
    glLinkProgram(_program);

    // Fehlerprüfung
    GLint link_status;
    glGetProgramiv(_program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        GLint logLen;
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetProgramInfoLog(_program, logLen, NULL, log.data());
        std::cerr << "Shader Program Link Error (" << _name << "): " << log.data() << std::endl;
    }
}
// --- ENDE DER NEUEN FUNKTION ---

std::string Drawable::loadShaderFile(std::string path) const
{
    // Nutze Qt-Ressourcensystem, um Shader-Datei zu laden
    QFile f(QString::fromStdString(path));
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        std::cerr << "Could not load shader file: " << path << std::endl;
        return "";
    }
    QTextStream in(&f);
    return in.readAll().toStdString();
}

GLuint Drawable::loadTexture(std::string path)
{
    // Nutze Qt, um Textur zu laden
    QImage tex;
    tex.load(QString::fromStdString(path));
    tex = QGLWidget::convertToGLFormat(tex);

    if(tex.isNull()){
        std::cerr << "Could not load texture file: " << path << std::endl;
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