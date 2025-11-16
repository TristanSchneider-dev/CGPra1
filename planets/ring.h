#ifndef RING_H
#define RING_H

#include "planets/drawable.h"
#include <memory>
#include <string>

// Forward declarations
class Sun;
class Cone;

/**
 * @brief Die Ring-Klasse stellt einen flachen, texturierten Ring dar (z.B. für Saturn).
 *
 * Erbt von Drawable und implementiert eine eigene Geometrieerstellung und Shader-Logik
 * für Transparenz.
 */
class Ring : public Drawable
{
public:
    /**
     * @brief Konstruktor für einen Ring.
     * @param name Name des Objekts.
     * @param innerRadius Der innere Radius (wo der Ring "leer" ist).
     * @param outerRadius Der äußere Radius (bis wohin der Ring geht).
     * @param textureLocation Pfad zur Textur (z.B. ":/res/images/ring.bmp").
     * @param axialTilt Die axiale Neigung des Rings (z.B. 26.7 Grad für Saturn).
     */
    Ring(std::string name,
         float innerRadius,
         float outerRadius,
         std::string textureLocation,
         float axialTilt);

    virtual void init() override;
    virtual void draw(glm::mat4 projection_matrix) const override;
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    /**
     * @brief Übergibt die Lichtquellen an den Ring, damit dieser beleuchtet werden kann.
     */
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