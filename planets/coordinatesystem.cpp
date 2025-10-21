#include <GL/glew.h>

#include "coordinatesystem.h"

#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glbase/gltool.hpp"

#include "gui/config.h"

CoordinateSystem::CoordinateSystem(std::string name) :
    Drawable(name)
{

}

void CoordinateSystem::draw(glm::mat4 projection_matrix) const
{
    /// TODO: your code here
}

void CoordinateSystem::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    /// TODO: your code here
}

std::string CoordinateSystem::getVertexShader() const
{
    /// TODO: your code here
    return "";
}

std::string CoordinateSystem::getFragmentShader() const
{
    /// TODO: your code here
    return "";
}

void CoordinateSystem::createObject()
{
    /// TODO: your code here
}
