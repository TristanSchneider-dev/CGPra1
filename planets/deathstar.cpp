#include "deathstar.h"
#include "planets/cone.h"
#include <glm/gtx/transform.hpp>
#include <stack>
#include "gui/config.h"
#include "glbase/gltool.hpp"

#include "planets/orbit.h"
#include "planets/path.h"

#include <QDebug>

DeathStar::DeathStar(std::string name, float radius, float distance, float hoursPerDay, float daysPerYear, std::string textureLocation,
                     float startAngle, float inclination) :
    Planet(name, radius, distance, hoursPerDay, daysPerYear, textureLocation,
           startAngle, inclination)
{
    qDebug() << "DeathStar constructor called:" << QString::fromStdString(name);
    _cone = std::make_shared<Cone>("Death Ray", radius);
}

void DeathStar::init()
{
    qDebug() << "DeathStar::init() called for" << QString::fromStdString(_name);
    Planet::init();
    if (_cone)
        _cone->init();
}

void DeathStar::recreate()
{
    qDebug() << "DeathStar::recreate() called for" << QString::fromStdString(_name);
    Planet::recreate();
    if (_cone)
        _cone->recreate();
}

void DeathStar::setResolution(unsigned int segments)
{
    qDebug() << "DeathStar::setResolution() called with segments:" << segments;
    Planet::setResolution(segments);
    if (_cone)
        _cone->setResolution(segments);
}

void DeathStar::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
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

    if (_cone)
        _cone->update(elapsedTimeMs, _modelViewMatrix);

    for (const auto& child : _children)
    {
        child->update(elapsedTimeMs, _modelViewMatrix);
    }
}

void DeathStar::draw(glm::mat4 projection_matrix) const
{
    Planet::draw(projection_matrix);

    if (_cone)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);

        _cone->draw(projection_matrix);

        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }
}

std::shared_ptr<Cone> DeathStar::cone() const
{
    return _cone;
}