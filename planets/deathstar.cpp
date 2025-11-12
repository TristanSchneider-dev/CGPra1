#include "deathstar.h"
#include "planets/cone.h"

// 1. Konstruktor anpassen
DeathStar::DeathStar(std::string name, float radius, float distance, float hoursPerDay, float daysPerYear, std::string textureLocation,
                     float startAngle, float inclination) : // NEUE Parameter
    Planet(name, radius, distance, hoursPerDay, daysPerYear, textureLocation,
           startAngle, inclination) // NEU: Weiterleitung an die Basisklasse
{
    // Der Kegel (Laser) wird erstellt
    _cone = std::make_shared<Cone>("Death Ray", radius);
}

// 2. Implementierung der überschriebenen Funktionen

void DeathStar::init()
{
    // WICHTIG: Zuerst die Basisklasse (Planet) initialisieren
    Planet::init();
    // Dann den eigenen Kegel initialisieren
    if (_cone)
        _cone->init();
}

void DeathStar::recreate()
{
    // Basisklasse neu erstellen
    Planet::recreate();
    // Eigenen Kegel neu erstellen
    if (_cone)
        _cone->recreate();
}

void DeathStar::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    // 1. Basisklasse updaten
    // Dies bewegt den Todesstern (Planet::update) und berechnet seine _modelViewMatrix
    Planet::update(elapsedTimeMs, modelViewMatrix);

    // 2. Jetzt den Kegel updaten, und zwar mit der *neuen* Matrix des Todessterns
    //    _modelViewMatrix ist die finale Matrix des Todessterns
    if (_cone)
        _cone->update(elapsedTimeMs, _modelViewMatrix);
}

void DeathStar::draw(glm::mat4 projection_matrix) const
{
    // 1. Basisklasse zeichnen
    // Dies zeichnet den Todesstern-Ball und alle seine Kinder (falls vorhanden)
    Planet::draw(projection_matrix);

    // 2. Jetzt den Kegel (Laser) zeichnen
    if (_cone)
        _cone->draw(projection_matrix);
}

std::shared_ptr<Cone> DeathStar::cone() const
{
    // Den erstellten Kegel zurückgeben
    return _cone;
}