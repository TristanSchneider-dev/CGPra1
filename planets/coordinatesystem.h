#ifndef COORDINATESYSTEM_H
#define COORDINATESYSTEM_H

#include "planets/drawable.h"
#include <glm/mat4x4.hpp>
#include <GL/glew.h>

class CoordinateSystem : public Drawable
{
public:
    CoordinateSystem(std::string name = "Coordinate System");

public:
    virtual void draw(glm::mat4 projection_matrix) const override;
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

protected:
    virtual void initShader() override;
    virtual std::string getVertexShader() const override;
    virtual std::string getFragmentShader() const override;
    virtual void createObject() override;

protected:
    GLuint _vbo_vertices = 0;
    GLsizei _verticesCount = 0;
};

#endif // COORDINATESYSTEM_H