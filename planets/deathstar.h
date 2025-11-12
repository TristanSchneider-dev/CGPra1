#ifndef DEATHSTAR_H
#define DEATHSTAR_H

#include "planets/planet.h"

class Cone;

class DeathStar : public Planet
{
public:
    DeathStar(std::string name = "UNNAMED DEATH STAR",
              float radius = 1.0f,
              float distance = 10.0f,
              float hoursPerDay = 24.0f,
              float daysPerYear = 365.0f,
              std::string textureLocation = ":/res/images/moon.bmp",
              float startAngle = 0.0f,
              float inclination = 0.0f);

    virtual void init() override;
    virtual void recreate() override;

    // WICHTIG: Überschreiben
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    virtual void draw(glm::mat4 projection_matrix) const override;
    std::shared_ptr<Cone> cone() const;

protected:
    std::shared_ptr<Cone> _cone;
};

#endif // DEATHSTAR_H