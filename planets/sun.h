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
        float startAngle = 0.0f,    // NEU
        float inclination = 0.0f); // NEU

    /**
     * @brief getPosition Getter for the position
     * @return the position in model view coordinates
     */
    glm::vec3 getPosition() const;

protected:
    // NEU: Überschreiben der Shader-Methoden von Planet
    /**
     * @see Drawable::getVertexShader()
     */
    virtual std::string getVertexShader() const override;

    /**
     * @see Drawable::getFragmentShader()
     */
    virtual std::string getFragmentShader() const override;

};

#endif // SUN_H