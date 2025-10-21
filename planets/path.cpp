#include <GL/glew.h>
#include "path.h"

#include <glm/gtc/type_ptr.hpp>

#include "glbase/gltool.hpp"

#include "gui/config.h"

Path::Path(std::string name):
    Drawable(name)
{

}

void Path::draw(glm::mat4 projection_matrix) const
{
    /// TODO: your code here
}

void Path::createObject()
{
    /// TODO: your code here
}

void Path::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    /// TODO: your code here
}

void Path::addPosition(glm::vec3 position)
{
    /// TODO: your code here
}

std::string Path::getVertexShader() const
{
    /// TODO: your code here
    return "";
}

std::string Path::getFragmentShader() const
{
    /// TODO: your code here
    return "";
}
