#include <GL/glew.h>

#include "skybox.h"

#include <glm/gtc/type_ptr.hpp>

#include "glbase/gltool.hpp"


Skybox::Skybox(std::string name): Drawable(name)
{
}

void Skybox::init()
{
    Drawable::init();
    /// TODO: your code here
}

void Skybox::draw(glm::mat4 projection_matrix) const
{
    /// TODO: your code here
}

void Skybox::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    /// TODO: your code here
}

std::string Skybox::getVertexShader() const
{
    /// TODO: your code here
    return "";
}

std::string Skybox::getFragmentShader() const
{
    /// TODO: your code here
    return "";
}

void Skybox::loadTexture()
{
    /// TODO: your code here
    // Hint: you can use GL_TEXTURE_CUBE_MAP instead of default textures
}

void Skybox::createObject()
{
    ///TODO: your code here
}

