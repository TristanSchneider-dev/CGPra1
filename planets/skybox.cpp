#include <GL/glew.h>

#include "skybox.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat3x3.hpp>

#include "glbase/gltool.hpp"
#include "image/image.h"

#include <vector>
#include <iostream>

namespace {
    GLuint s_cubemapTextureID = 0;
}


Skybox::Skybox(std::string name): Drawable(name)
{
}

void Skybox::init()
{
    Drawable::init();
    loadTexture();
}

void Skybox::draw(glm::mat4 projection_matrix) const
{
    // === 1. OpenGL-Status sichern ===
    GLint currentDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &currentDepthFunc);

    GLboolean isCulling;
    glGetBooleanv(GL_CULL_FACE, &isCulling);
    // (Man könnte auch glPushAttrib/glPopAttrib verwenden, aber das ist 'deprecated')


    // === 2. Status für Skybox setzen ===
    glDepthFunc(GL_LEQUAL);     // Wichtig für den .xyww Shader-Trick
    glDisable(GL_CULL_FACE);  // Wichtig, da wir *im* Würfel sind

    glUseProgram(_program);

    // Uniforms setzen
    GLint viewLoc = glGetUniformLocation(_program, "view");
    GLint projLoc = glGetUniformLocation(_program, "projection");
    GLint skyboxLoc = glGetUniformLocation(_program, "skybox");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    // Textur binden
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, s_cubemapTextureID);
    glUniform1i(skyboxLoc, 0);

    // Zeichnen
    glBindVertexArray(_vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    // === 3. Ursprünglichen OpenGL-Status wiederherstellen ===
    glDepthFunc(currentDepthFunc); // Originale Tiefenfunktion zurück

    if (isCulling) // Culling nur aktivieren, wenn es vorher auch an war
    {
        glEnable(GL_CULL_FACE);
    }
}

void Skybox::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    // Translation aus der View-Matrix entfernen
    _modelViewMatrix = glm::mat4(glm::mat3(modelViewMatrix));
}

std::string Skybox::getVertexShader() const
{
    // Stelle sicher, dass diese Datei den .xyww Trick verwendet!
    return loadShaderFile(":/shader/skybox.vs.glsl");
}

std::string Skybox::getFragmentShader() const
{
    return loadShaderFile(":/shader/skybox.fs.glsl");
}

void Skybox::loadTexture()
{
    glGenTextures(1, &s_cubemapTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, s_cubemapTextureID);

    std::vector<std::string> faces = {
        ":/shader/skybox/px.png",
        ":/shader/skybox/nx.png",
        ":/shader/skybox/py.png",
        ":/shader/skybox/ny.png",
        ":/shader/skybox/pz.png",
        ":/shader/skybox/nz.png"
    };

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        Image image(faces[i]);
        if (image.getData())
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getData());
        }
        else
        {
            std::cerr << "Cubemap Textur konnte nicht geladen werden: " << faces[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::createObject()
{
    GLfloat skyboxVertices[] = {
        // ... (die 36 Vertices von vorhin, hier gekürzt)
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindVertexArray(0);
}