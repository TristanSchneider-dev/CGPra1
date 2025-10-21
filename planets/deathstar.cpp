#include "deathstar.h"

#include "planets/cone.h"

DeathStar::DeathStar(std::string name, float radius, float distance, float hoursPerDay, float daysPerYear, std::string textureLocation):
    Planet(name, radius, distance, hoursPerDay, daysPerYear, textureLocation)
{
    _cone = std::make_shared<Cone>("Death Ray", radius);
}

void DeathStar::init()
{
    /// TODO: your code here
}

void DeathStar::recreate()
{
    /// TODO: your code here
}

void DeathStar::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    /// TODO: your code here
}

void DeathStar::draw(glm::mat4 projection_matrix) const
{
    /// TODO: your code here
}

std::shared_ptr<Cone> DeathStar::cone() const
{
    /// TODO: your code here
    return nullptr;
}


