#ifndef ORBIT_H
#define ORBIT_H

#include "planets/drawable.h"

class Orbit : public Drawable
{
public:
    Orbit(std::string name = "UNKNOWN ORBIT", float radius = 1.f);

public:
    virtual void draw(glm::mat4 projection_matrix) const override;

    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

protected:
    virtual std::string getVertexShader() const override;

    virtual std::string getFragmentShader() const override;

    virtual void createObject() override;

    float _radius;
    unsigned int _indexCount = 0;
};

#endif // ORBIT_H