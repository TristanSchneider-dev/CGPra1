#ifndef SUN_H
#define SUN_H

#include "planets/planet.h"
#include <glm/vec3.hpp>

class Sun : public Planet
{
public:
    Sun(std::string name = "Sun",
        float radius = 1.f,
        float distance = 1.f,
        float hoursPerDay = 250.f,
        float daysPerYear = 365.f,
        std::string textureLocation = ":/res/images/sun.bmp",
        float startAngle = 0.0f,
        float inclination = 0.0f);

    glm::vec3 getPosition() const;

    // --- NEU HINZUGEFÜGT ---
    /**
     * @see Drawable::update(float, glm::mat4)
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

protected:
    virtual std::string getVertexShader() const override;
    virtual std::string getFragmentShader() const override;
};

#endif // SUN_H