#ifndef CONE_H
#define CONE_H

#include "planets/drawable.h"

#include <glm/vec3.hpp>

class Cone : public Drawable
{
public:
    Cone(std::string name, float distance);

public:
    virtual void draw(glm::mat4 projection_matrix) const override;

    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    float getAngle() const;

    glm::vec3 getPosition() const;

    glm::vec3 getDirection() const;

protected:

    virtual std::string getVertexShader() const override;

    virtual std::string getFragmentShader() const override;

    virtual void createObject() override;

    float _distance;
    float _angle;
    glm::vec3 _position;
    glm::vec3 _direction;

    unsigned int _indexCount = 0;
};

#endif // CONE_H