#include <GL/glew.h>

#include "orbit.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glbase/gltool.hpp"

#include "gui/config.h"

Orbit::Orbit(std::string name, float radius):
    Drawable(name),
    _radius(radius)
{

}

void Orbit::draw(glm::mat4 projection_matrix) const
{
    /// TODO: your code here
}

void Orbit::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    /// TODO: your code here
}

std::string Orbit::getVertexShader() const
{
   /// TODO: your code here
   return "";
}

std::string Orbit::getFragmentShader() const
{
    /// TODO: your code here
    return "";
}

void Orbit::createObject()
{
   /// TODO: your code here
}
