#include <GL/glew.h>
#include "sun.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <stack>
#include "glbase/gltool.hpp"
#include "gui/config.h"

#include "planets/orbit.h"
#include "planets/path.h"

#include <QDebug>

Sun::Sun(std::string name, float radius, float distance, float hoursPerDay, float daysPerYear, std::string textureLocation,
         float startAngle, float inclination):
    Planet(name, radius, distance, hoursPerDay, daysPerYear, textureLocation,
           startAngle, inclination)
{
    qDebug() << "Sun constructor called for:" << QString::fromStdString(_name);
}

glm::vec3 Sun::getPosition() const
{
    return glm::vec3(_modelViewMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

void Sun::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    glm::mat4 baseOperatingMatrix;
    if (Config::show3DOrbits)
        baseOperatingMatrix = glm::rotate(modelViewMatrix, glm::radians(_inclination), glm::vec3(0.0f, 0.0f, 1.0f));
    else
        baseOperatingMatrix = modelViewMatrix;

    _orbit->update(elapsedTimeMs, baseOperatingMatrix);
    _path->update(elapsedTimeMs, modelViewMatrix);

    float elapsedSimulatedDays = (elapsedTimeMs / 60000.0f) * Config::animationSpeed;

    if(Config::localRotation)
        _localRotation += elapsedSimulatedDays * _localRotationSpeed;

    if (Config::GlobalRotation)
        _globalRotation += elapsedSimulatedDays * _globalRotationSpeed;

    while(_globalRotation >= 360.f) _globalRotation -= 360.0f;
    while(_globalRotation < 0.0f) _globalRotation += 360.0f;
    while(_localRotation >= 360.f) _localRotation -= 360.0f;
    while(_localRotation < 0.0f) _localRotation += 360.0f;

    std::stack<glm::mat4> modelview_stack;
    modelview_stack.push(baseOperatingMatrix);
        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(_globalRotation), glm::vec3(0,1,0));
        modelview_stack.top() = glm::translate(modelview_stack.top(), glm::vec3(_distance, 0, 0));
        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(_localRotation), glm::vec3(0,1,0));
        _modelViewMatrix = glm::mat4(modelview_stack.top());
    modelview_stack.pop();

    bool originalGlobalRotationState = Config::GlobalRotation;
    Config::GlobalRotation = Config::localOrbits;

    for (const auto& child : _children)
    {
        child->update(elapsedTimeMs, _modelViewMatrix);
    }

    Config::GlobalRotation = originalGlobalRotationState;
}

std::string Sun::getVertexShader() const
{
    return Drawable::loadShaderFile(":/shader/sun.vs.glsl");
}

std::string Sun::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/sun.fs.glsl");
}