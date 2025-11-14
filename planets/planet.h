#ifndef PLANET_H
#define PLANET_H

#include "planets/drawable.h"

#include <memory>
#include <vector>

class Orbit;
class Path;
class Sun;
class Cone;

class Planet : public Drawable
{
public:
    Planet(std::string name = "UNNAMED PLANET",
            float radius = 1.0f,
            float distance = 10.0f,
            float hoursPerDay = 24.0f,
            unsigned int daysPerYear = 365,
            std::string textureLocation = ":/res/images/earth.bmp",
            float startAngle = 0.0f,
            float inclination = 0.0f
            );

    virtual void init() override;
    virtual void recreate() override;
    virtual void draw(glm::mat4 projection_matrix) const override;
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    virtual void setLights(std::shared_ptr<Sun> sun, std::shared_ptr<Cone> laser);
    virtual void addChild(std::shared_ptr<Planet> child);
    virtual void calculatePath(glm::mat4 modelViewMatrix);

    virtual void setResolution(unsigned int segments) override;

    // NEU: Setter für die Wolkentextur
    virtual void setCloudTexture(std::string textureLocation);

    ~Planet();

protected:
    std::vector<std::shared_ptr<Planet>> _children;

    float _radius;
    float _distance;
    float _inclination;

    unsigned int _daysPerYear;

    float _localRotation;
    float _localRotationSpeed;

    float _globalRotation;
    float _globalRotationSpeed;

    float _totalTimeMs = 0.0f; // NEU: Für Animation

    unsigned int _indexCount = 0;

    std::string _textureLocation;
    GLuint _textureID = 0;

    // NEU: Variablen für Wolkentextur
    std::string _cloudTextureLocation;
    GLuint _cloudTextureID = 0;

    std::shared_ptr<Orbit> _orbit;
    std::shared_ptr<Path> _path;

    std::shared_ptr<Sun> _sun;
    std::shared_ptr<Cone> _laser;

    virtual void createObject() override;
    virtual std::string getVertexShader() const override;
    virtual std::string getFragmentShader() const override;

    unsigned int getCommonYears(unsigned int other);
    unsigned int greatestCommonDivisor(unsigned int a, unsigned int b);

    virtual void updatePath(float elapsedSimulatedDays, glm::mat4 modelViewMatrix);
    virtual void addPathPoint();
    virtual void createPath();
};

#endif // PLANET_H