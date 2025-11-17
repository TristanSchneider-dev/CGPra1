#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <string>

#define GLM_FORCE_RADIANS
#include <glm/mat4x4.hpp>

#ifdef _WIN32
    #include <windows.h>
#endif

#include <GL/glew.h>

class Cone;
class Sun;

class Drawable{

public:

    Drawable(std::string name = "UNNAMED");

    virtual void init();

    virtual void recreate();

    virtual void draw(glm::mat4 projection_matrix) const = 0;

    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) = 0;

    virtual void setResolution(unsigned int segments);

protected:

    std::string _name;

    GLuint _program;
    glm::mat4 _modelViewMatrix;

    unsigned int _resolutionSegments;

    GLuint _vertexArrayObject;
    GLuint _positionBuffer;
    GLuint _normalBuffer;
    GLuint _texCoordBuffer;
    GLuint _indexBuffer;

    virtual void initShader();

    virtual std::string loadShaderFile(std::string path) const;

    virtual GLuint loadTexture(std::string path);

    virtual std::string getVertexShader() const = 0;

    virtual std::string getFragmentShader() const = 0;

    virtual void createObject() = 0;
};

#endif // DRAWABLE_H