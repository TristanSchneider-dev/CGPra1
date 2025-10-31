#include <GL/glew.h>

#include "sun.h"

#include <glm/gtc/type_ptr.hpp> // Für glm::vec4
#include "glbase/gltool.hpp"

Sun::Sun(std::string name, float radius, float distance, float hoursPerDay, float daysPerYear, std::string textureLocation):
    Planet(name, radius, distance, hoursPerDay, daysPerYear, textureLocation)
{
    // Da die Sonne ein Planet ist, wird createObject(), init() etc.
    // automatisch von der Planet-Klasse aufgerufen.
}

glm::vec3 Sun::getPosition() const
{
    // Die Position der Sonne ist ihr lokaler Ursprung (0,0,0)
    // transformiert mit ihrer ModelView-Matrix.
    // Die _modelViewMatrix wird in der update()-Methode der
    // Basisklasse (Planet) gesetzt.
    return glm::vec3(_modelViewMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

// NEU: Implementierung der überschriebenen Methoden

std::string Sun::getVertexShader() const
{
    return Drawable::loadShaderFile(":/shader/sun.vs.glsl");
}

std::string Sun::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/sun.fs.glsl");
}