#include <GL/glew.h>
#include "planet.h"


#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

#include "glbase/gltool.hpp"

#include "gui/config.h"

#include "planets/cone.h"
#include "planets/sun.h" // Wichtig für die Beleuchtung
#include "planets/orbit.h"
#include "planets/path.h"

Planet::Planet(std::string name,
               float radius,
               float distance,
               float hoursPerDay,
               unsigned int daysPerYear,
               std::string textureLocation):
    Drawable(name),
    _radius(radius),
    _distance(distance),
    _localRotation(0),
    _localRotationSpeed(0),
    _daysPerYear(daysPerYear),
    _textureLocation(textureLocation),
    _textureID(0),
    _globalRotation(0)
{
    // Definiere Geschwindigkeiten in "Grad pro Tag"

    // Lokale Geschwindigkeit: (Grad pro simuliertem Tag)
    if (hoursPerDay > 0.0f)
        _localRotationSpeed = (24.0f / hoursPerDay) * 360.0f;
    else
        _localRotationSpeed = 0.0f;

    // Globale Geschwindigkeit: (Grad pro simuliertem Tag)
    if (daysPerYear > 0)
        _globalRotationSpeed = 360.0f / daysPerYear;
    else
        _globalRotationSpeed = 0.0f;

    _orbit = std::make_shared<Orbit>(name + " Orbit", _distance);
    _path = std::make_shared<Path>(name + " Pfad");
}

void Planet::init()
{
    Drawable::init();

    // Textur laden
    if (!_textureLocation.empty())
    {
        _textureID = loadTexture(_textureLocation);
        if (_textureID == 0)
        {
            std::cerr << "Could not load texture for " << _name << " from " << _textureLocation << std::endl;
        }
    }

    // REKURSION: Kinder und zugehörige Objekte initialisieren
    _orbit->init();
    _path->init();
    for (const auto& child : _children)
    {
        child->init();
    }
}

void Planet::recreate()
{
    Drawable::recreate();

    // REKURSION: Kinder und zugehörige Objekte neu erstellen
    _orbit->recreate();
    _path->recreate();
    for (const auto& child : _children)
    {
        child->recreate();
    }
}



void Planet::draw(glm::mat4 projection_matrix) const
{
    // REKURSION: Zuerst Orbit, Pfad und Kinder zeichnen
    _orbit->draw(projection_matrix);
    _path->draw(projection_matrix);
    for (const auto& child : _children)
    {
        child->draw(projection_matrix);
    }

    // --- Jetzt das eigentliche Planeten-Objekt zeichnen ---

    if(_program == 0){
        std::cerr << "Planet" << _name << "not initialized. Call init() first." << std::endl;
        return;
    }

    // Load program
    glUseProgram(_program);

    // bin vertex array object
    glBindVertexArray(_vertexArrayObject);

    // Texturen und Licht-Uniforms setzen

    // 1. Textur binden
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glUniform1i(glGetUniformLocation(_program, "uTextureSampler"), 0);

    // 2. Licht-Uniforms setzen
    glm::vec3 lightPosView = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Standard-Licht

    if (Config::sunLight && _sun)
    {
        lightPosView = _sun->getPosition();
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Weißes Licht
    }

    glUniform3fv(glGetUniformLocation(_program, "uLightPosView"), 1, glm::value_ptr(lightPosView));
    glUniform3fv(glGetUniformLocation(_program, "uLightColor"), 1, glm::value_ptr(lightColor));


    // set parameter
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

    // call draw
    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);

    // unbin vertex array object
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // check for errors
    VERIFY(CG::checkError());
}

void Planet::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    // REKURSION: Orbit und Pfad updaten (mit der Matrix des Parents)
    _orbit->update(elapsedTimeMs, modelViewMatrix);
    _path->update(elapsedTimeMs, modelViewMatrix);

    // KORRIGIERT: Basisgeschwindigkeit 60x langsamer gemacht
    // Annahme: Config::animationSpeed = 1.0 -> 1 simulierter Tag pro reale Minute (60000ms)
    float elapsedSimulatedDays = (elapsedTimeMs / 60000.0f) * Config::animationSpeed;

    // 1. Globale Rotation (Orbit) berechnen
    if (Config::GlobalRotation)
        _globalRotation += elapsedSimulatedDays * _globalRotationSpeed;
    while(_globalRotation >= 360.f) _globalRotation -= 360.0f;
    while(_globalRotation < 0.0f) _globalRotation += 360.0f;

    // 2. Lokale Rotation (Drehung) berechnen
    if(Config::localRotation)
        _localRotation += elapsedSimulatedDays * _localRotationSpeed;
    while(_localRotation >= 360.f) _localRotation -= 360.0f;
    while(_localRotation < 0.0f) _localRotation += 360.0f;


    // Eigene ModelView-Matrix berechnen
    std::stack<glm::mat4> modelview_stack;
    modelview_stack.push(modelViewMatrix);

        // 1. Rotiere um den Parent (z.B. Sonne) auf der Y-Achse
        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(_globalRotation), glm::vec3(0,1,0));
        // 2. Schiebe auf die Distanz (Orbit-Radius) entlang der X-Achse
        modelview_stack.top() = glm::translate(modelview_stack.top(), glm::vec3(_distance, 0, 0));
        // 3. Wende die lokale Rotation an (Drehung um sich selbst)
        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(_localRotation), glm::vec3(0,1,0));

        _modelViewMatrix = glm::mat4(modelview_stack.top());

    modelview_stack.pop();

    // REKURSION: Kinder updaten (mit der *neuen* Matrix dieses Planeten)
    for (const auto& child : _children)
    {
        child->update(elapsedTimeMs, _modelViewMatrix);
    }
}

void Planet::setLights(std::shared_ptr<Sun> sun, std::shared_ptr<Cone> laser)
{
    _sun = sun;
    _laser = laser;
    // REKURSION: Lichter an Kinder weitergeben
    for(auto child : _children)
        child->setLights(sun, laser);
}

void Planet::addChild(std::shared_ptr<Planet> child)
{
    _children.push_back(child);
}


void Planet::createObject(){

    // Auflösung der Kugel
    unsigned int latitudeSegments = 30;
    unsigned int longitudeSegments = 30;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i <= latitudeSegments; ++i)
    {
        float v = (float)i / latitudeSegments; // 0.0 (Südpol) bis 1.0 (Nordpol)
        float latitudeAngle = glm::radians(-90.0f + v * 180.0f); // -90 bis +90 Grad

        for (unsigned int j = 0; j <= longitudeSegments; ++j)
        {
            float u = (float)j / longitudeSegments; // 0.0 bis 1.0
            float longitudeAngle = glm::radians(u * 360.0f); // 0 bis 360 Grad

            // Position (Y ist oben)
            float x = _radius * cos(latitudeAngle) * cos(longitudeAngle);
            float y = _radius * sin(latitudeAngle);
            float z = _radius * cos(latitudeAngle) * sin(longitudeAngle);
            positions.push_back(glm::vec3(x, y, z));

            // Normale
            normals.push_back(glm::normalize(glm::vec3(x, y, z)));

            // Texturkoordinaten
            texCoords.push_back(glm::vec2(u, 1.0f - v));
        }
    }

    // Indizes
    for (unsigned int i = 0; i < latitudeSegments; ++i)
    {
        for (unsigned int j = 0; j < longitudeSegments; ++j)
        {
            unsigned int v1 = (i * (longitudeSegments + 1)) + j;
            unsigned int v2 = v1 + 1;
            unsigned int v3 = ((i + 1) * (longitudeSegments + 1)) + j;
            unsigned int v4 = v3 + 1;

            indices.push_back(v1);
            indices.push_back(v3);
            indices.push_back(v2);

            indices.push_back(v2);
            indices.push_back(v3);
            indices.push_back(v4);
        }
    }

    _indexCount = static_cast<unsigned int>(indices.size());

    if(_vertexArrayObject == 0)
        glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    // VBOs
    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // layout (location = 0)
    glEnableVertexAttribArray(0);

    GLuint normal_buffer;
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); // layout (location = 1)
    glEnableVertexAttribArray(1);

    GLuint texcoord_buffer;
    glGenBuffers(1, &texcoord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, texcoord_buffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); // layout (location = 2)
    glEnableVertexAttribArray(2);

    // Index-Buffer
    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    VERIFY(CG::checkError());
}

std::string Planet::getVertexShader() const
{
    // Verwendet den Beleuchtungs-Shader
    return Drawable::loadShaderFile(":/shader/phong.vs.glsl");

}

std::string Planet::getFragmentShader() const
{
    // Verwendet den Beleuchtungs-Shader
    return Drawable::loadShaderFile(":/shader/phong.fs.glsl");
}

Planet::~Planet(){
}


/*************************************************
 * Hint: You don't need to change the code below *
 * KORRIGIERTE PATH-LOGIK
 *************************************************/

void Planet::calculatePath(glm::mat4 modelViewMatrix)
{
    // Diese Funktion berechnet den Pfad für alle Kinder
    for(auto child : _children){
        unsigned int longestCommonMultiple = child->getCommonYears(_daysPerYear);
        // Simuliere 'longestCommonMultiple' Tage
        for(unsigned int i = 0; i <= longestCommonMultiple; i++){
            // Simuliere einen Tag pro Schritt
            float elapsedSimulatedDays = 1.0f;
            child->updatePath(elapsedSimulatedDays, modelViewMatrix);
        }
    }
     createPath();
}


unsigned int Planet::getCommonYears(unsigned int other)
{
    unsigned int tmp = other * _daysPerYear / greatestCommonDivisor(other, _daysPerYear);
    unsigned int result = tmp;
    for(auto child : _children){
        result = std::max(result, child->getCommonYears(tmp));
    }
    return result;
}

unsigned int Planet::greatestCommonDivisor(unsigned int a, unsigned int b)
{
    if (b == 0)
        return a;
    else
        return greatestCommonDivisor(b, a % b);
}

// KORRIGIERT: Parametername und Logik
void Planet::updatePath(float elapsedSimulatedDays, glm::mat4 modelViewMatrix)
{
    // Diese Funktion simuliert die Bewegung basierend auf 'elapsedSimulatedDays'
    // und ruft SICH SELBST rekursiv für Kinder auf.
    // Sie ruft NICHT die normale update() Methode auf.

    // 1. Globale Rotation (Orbit) berechnen
    _globalRotation += elapsedSimulatedDays * _globalRotationSpeed;
    while(_globalRotation >= 360.f) _globalRotation -= 360.0f;
    while(_globalRotation < 0.0f) _globalRotation += 360.0f;

    // 2. Lokale Rotation (Drehung) berechnen
    _localRotation += elapsedSimulatedDays * _localRotationSpeed;
    while(_localRotation >= 360.f) _localRotation -= 360.0f;
    while(_localRotation < 0.0f) _localRotation += 360.0f;

    // Eigene ModelView-Matrix berechnen
    std::stack<glm::mat4> modelview_stack;
    modelview_stack.push(modelViewMatrix);
        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(_globalRotation), glm::vec3(0,1,0));
        modelview_stack.top() = glm::translate(modelview_stack.top(), glm::vec3(_distance, 0, 0));
        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(_localRotation), glm::vec3(0,1,0));
        _modelViewMatrix = glm::mat4(modelview_stack.top());
    modelview_stack.pop();

    // Pfadpunkt für DIESEN Planeten hinzufügen
    addPathPoint();

    // REKURSION: Path-Update für Kinder aufrufen
    for (const auto& child : _children)
    {
        child->updatePath(elapsedSimulatedDays, _modelViewMatrix);
    }
}

void Planet::addPathPoint(){
    // KORRIGIERT: Diese Funktion ist NICHT MEHR REKURSIV
    _path->addPosition(glm::vec3(_modelViewMatrix * glm::vec4(0,0,0,1)));
}

void Planet::createPath(){
    _path->recreate();
    // Diese Funktion ist weiterhin rekursiv
    for(auto child : _children)
        child->createPath();
}