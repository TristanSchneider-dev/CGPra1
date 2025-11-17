#ifndef RING_H
#define RING_H

#include "planets/drawable.h"
#include <memory>
#include <string>

class Sun;
class Cone;

class Ring : public Drawable
{
public:
    Ring(std::string name,
         float innerRadius,
         float outerRadius,
         std::string textureLocation,
         float axialTilt);

    virtual void init() override;
    virtual void draw(glm::mat4 projection_matrix) const override;
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    virtual void setLights(std::shared_ptr<Sun> sun, std::shared_ptr<Cone> laser);

protected:
    virtual void createObject() override;
    virtual std::string getVertexShader() const override;
    virtual std::string getFragmentShader() const override;

    float _innerRadius;
    float _outerRadius;
    float _axialTilt;

    std::string _textureLocation;
    GLuint _textureID = 0;

    unsigned int _indexCount = 0;

    std::shared_ptr<Sun> _sun;
    std::shared_ptr<Cone> _laser;
};

#endif // RING_H