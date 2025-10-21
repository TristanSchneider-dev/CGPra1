#include <GL/glew.h>

#include "cone.h"

#include <vector>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glbase/gltool.hpp"

#include "gui/config.h"

Cone::Cone(std::string name, float distance):
    Drawable(name),
    _distance(distance), _angle(.0f)
{

}



void Cone::draw(glm::mat4 projection_matrix) const
{
    /// TODO: your code here
}

void Cone::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    /// TODO: your code here
}

std::string Cone::getVertexShader() const
{
    /// TODO: your code here
    return "";
}

std::string Cone::getFragmentShader() const
{
    /// TODO: your code here
    return "";
}

void Cone::createObject()
{
    /// TODO: your code here
}

glm::vec3 Cone::getDirection() const
{
    return _direction;
}

glm::vec3 Cone::getPosition() const
{
    return _position;
}

float Cone::getAngle() const
{
    return _angle;
}

