#include <GL/glew.h>
#include "planet.h"
#include <stack>


#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

#include "glbase/gltool.hpp"
#include "gui/config.h" // WICHTIG
#include "planets/cone.h"
#include "planets/sun.h"
#include "planets/orbit.h"
#include "planets/path.h"
#include "planets/ring.h"


Planet::Planet(std::string name,
               float radius,
               float distance,
               float hoursPerDay,
               unsigned int daysPerYear,
               std::string textureLocation,
               float startAngle,
               float inclination):
    Drawable(name),
    _radius(radius),
    _distance(distance),
    _inclination(inclination),
    _localRotation(0),
    _localRotationSpeed(0),
    _daysPerYear(daysPerYear),
    _textureLocation(textureLocation),
    _textureID(0),
    _globalRotation(startAngle),
    _ring(nullptr)
{
    if (hoursPerDay > 0.0f)
        _localRotationSpeed = (24.0f / hoursPerDay) * 360.0f;
    else
        _localRotationSpeed = 0.0f;

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

    if (_ring)
        _ring->init();

    if (!_textureLocation.empty())
    {
        _textureID = loadTexture(_textureLocation);
        if (_textureID == 0)
        {
            std::cerr << "Could not load texture for " << _name << " from " << _textureLocation << std::endl;
        }
    }

    if (!_cloudTextureLocation.empty())
    {
        _cloudTextureID = loadTexture(_cloudTextureLocation);
        if (_cloudTextureID == 0)
        {
            std::cerr << "Could not load cloud texture for " << _name << " from " << _cloudTextureLocation << std::endl;
        }
    }

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

    if (_ring)
        _ring->recreate();

    _orbit->recreate();
    _path->recreate();
    for (const auto& child : _children)
    {
        child->recreate();
    }
}

void Planet::draw(glm::mat4 projection_matrix) const
{
    _orbit->draw(projection_matrix);
    _path->draw(projection_matrix);
    for (const auto& child : _children)
    {
        child->draw(projection_matrix);
    }

    if(_program == 0){
        std::cerr << "Planet" << _name << "not initialized. Call init() first." << std::endl;
        return;
    }

    glUseProgram(_program);
    glBindVertexArray(_vertexArrayObject);

    // --- TEXTUR-BINDING START ---

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glUniform1i(glGetUniformLocation(_program, "uTextureSampler"), 0);

    bool hasClouds = (_cloudTextureID != 0);
    if (hasClouds)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _cloudTextureID);
        glUniform1i(glGetUniformLocation(_program, "uCloudSampler"), 1);
    }
    glUniform1i(glGetUniformLocation(_program, "uHasClouds"), hasClouds);

    float timeInSeconds = _totalTimeMs / 1000.0f;
    glUniform1f(glGetUniformLocation(_program, "uTime"), timeInSeconds);

    // --- TEXTUR-BINDING ENDE ---

    // --- SONNENLICHT-UNIFORMS ---
    glm::vec3 lightPosView = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    if (Config::sunLight && _sun)
    {
        lightPosView = _sun->getPosition();
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    glUniform3fv(glGetUniformLocation(_program, "uLightPosView"), 1, glm::value_ptr(lightPosView));
    glUniform3fv(glGetUniformLocation(_program, "uLightColor"), 1, glm::value_ptr(lightColor));

    // --- NEU: LASER-UNIFORMS ---
    bool hasLaser = (_laser != nullptr);
    glUniform1i(glGetUniformLocation(_program, "uHasLaser"), hasLaser);
    if (hasLaser)
    {
        // Hole die Daten vom Cone-Objekt (das als _laser gespeichert ist)
        glm::vec3 laserPosView = _laser->getPosition();
        glm::vec3 laserDirView = _laser->getDirection();

        // Hole den Winkel aus der Config (in Grad) und konvertiere in Cosinus
        // Wir übergeben den Cosinus, weil das im Shader effizienter ist.
        float cutoffAngleRad = glm::radians(Config::laserCutoff);
        float cutoffCos = cos(cutoffAngleRad);

        glUniform3fv(glGetUniformLocation(_program, "uLaserPosView"), 1, glm::value_ptr(laserPosView));
        glUniform3fv(glGetUniformLocation(_program, "uLaserDirView"), 1, glm::value_ptr(laserDirView));
        glUniform1f(glGetUniformLocation(_program, "uLaserCutoffCos"), cutoffCos);

        // Laserfarbe ist Rot
        glUniform3f(glGetUniformLocation(_program, "uLaserColor"), 1.0f, 0.0f, 0.0f);
    }
    // --- ENDE NEU ---

    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);

    // Unbinden
    glBindVertexArray(0);

    if (hasClouds)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // --- Ring zeichnen (nach dem Planeten) ---
    if (_ring)
    {
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        _ring->draw(projection_matrix);

        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }

    VERIFY(CG::checkError());
}

void Planet::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    _totalTimeMs += elapsedTimeMs;
    glm::mat4 baseOperatingMatrix;

    if (Config::show3DOrbits)
    {
        baseOperatingMatrix = glm::rotate(modelViewMatrix, glm::radians(_inclination), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    else
    {
        baseOperatingMatrix = modelViewMatrix;
    }

    _orbit->update(elapsedTimeMs, baseOperatingMatrix);
    _path->update(elapsedTimeMs, modelViewMatrix);

    float elapsedSimulatedDays = (elapsedTimeMs / 60000.0f) * Config::animationSpeed;

    if (Config::GlobalRotation)
        _globalRotation += elapsedSimulatedDays * _globalRotationSpeed;

    while(_globalRotation >= 360.f) _globalRotation -= 360.0f;
    while(_globalRotation < 0.0f) _globalRotation += 360.0f;

    if(Config::localRotation)
        _localRotation += elapsedSimulatedDays * _localRotationSpeed;

    while(_localRotation >= 360.f) _localRotation -= 360.0f;
    while(_localRotation < 0.0f) _localRotation += 360.0f;

    std::stack<glm::mat4> modelview_stack;
    modelview_stack.push(baseOperatingMatrix);

        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(_globalRotation), glm::vec3(0,1,0));
        modelview_stack.top() = glm::translate(modelview_stack.top(), glm::vec3(_distance, 0, 0));

        if (_ring)
        {
            _ring->update(elapsedTimeMs, modelview_stack.top());
        }

        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(_localRotation), glm::vec3(0,1,0));

        _modelViewMatrix = glm::mat4(modelview_stack.top());

    modelview_stack.pop();

    for (const auto& child : _children)
    {
        child->update(elapsedTimeMs, _modelViewMatrix);
    }
}

void Planet::setResolution(unsigned int segments)
{
    Drawable::setResolution(segments);

    if (_orbit)
        _orbit->setResolution(segments);

    if (_ring)
        _ring->setResolution(segments);

    for (const auto& child : _children)
    {
        child->setResolution(segments);
    }
}

void Planet::setLights(std::shared_ptr<Sun> sun, std::shared_ptr<Cone> laser)
{
    _sun = sun;
    _laser = laser;

    if (_ring)
        _ring->setLights(sun, laser);

    for(auto child : _children)
        child->setLights(sun, laser);
}

void Planet::setRing(std::shared_ptr<Ring> ring)
{
    _ring = ring;
}

void Planet::addChild(std::shared_ptr<Planet> child)
{
    _children.push_back(child);
}


void Planet::createObject(){
    unsigned int latitudeSegments = _resolutionSegments;
    unsigned int longitudeSegments = _resolutionSegments;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i <= latitudeSegments; ++i)
    {
        float v = (float)i / latitudeSegments;
        float latitudeAngle = glm::radians(-90.0f + v * 180.0f);
        for (unsigned int j = 0; j <= longitudeSegments; ++j)
        {
            float u = (float)j / longitudeSegments;
            float longitudeAngle = glm::radians(u * 360.0f);
            float x = _radius * cos(latitudeAngle) * cos(longitudeAngle);
            float y = _radius * sin(latitudeAngle);
            float z = _radius * cos(latitudeAngle) * sin(longitudeAngle);
            positions.push_back(glm::vec3(x, y, z));
            normals.push_back(glm::normalize(glm::vec3(x, y, z)));
            texCoords.push_back(glm::vec2(u, 1.0f - v));
        }
    }
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

    if (_positionBuffer == 0)
        glGenBuffers(1, &_positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    if (_normalBuffer == 0)
        glGenBuffers(1, &_normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    if (_texCoordBuffer == 0)
        glGenBuffers(1, &_texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    if (_indexBuffer == 0)
        glGenBuffers(1, &_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    VERIFY(CG::checkError());
}

std::string Planet::getVertexShader() const
{
    return Drawable::loadShaderFile(":/shader/phong.vs.glsl");
}

std::string Planet::getFragmentShader() const
{
    return Drawable::loadShaderFile(":/shader/phong.fs.glsl");
}

Planet::~Planet(){
}

/*************************************************
 * Path-Logik (bleibt unverändert)
 *************************************************/

void Planet::calculatePath(glm::mat4 modelViewMatrix)
{
    for(auto child : _children){
        unsigned int longestCommonMultiple = child->getCommonYears(_daysPerYear);
        for(unsigned int i = 0; i <= longestCommonMultiple; i++){
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

void Planet::updatePath(float elapsedSimulatedDays, glm::mat4 modelViewMatrix)
{
    glm::mat4 orbitBaseMatrix = glm::rotate(modelViewMatrix, glm::radians(_inclination), glm::vec3(0.0f, 0.0f, 1.0f));

    _globalRotation += elapsedSimulatedDays * _globalRotationSpeed;
    while(_globalRotation >= 360.f) _globalRotation -= 360.0f;
    while(_globalRotation < 0.0f) _globalRotation += 360.0f;

    _localRotation += elapsedSimulatedDays * _localRotationSpeed;
    while(_localRotation >= 360.f) _localRotation -= 360.0f;
    while(_localRotation < 0.0f) _localRotation += 360.0f;

    std::stack<glm::mat4> modelview_stack;
    modelview_stack.push(orbitBaseMatrix);
        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(_globalRotation), glm::vec3(0,1,0));
        modelview_stack.top() = glm::translate(modelview_stack.top(), glm::vec3(_distance, 0, 0));
        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(_localRotation), glm::vec3(0,1,0));
        _modelViewMatrix = glm::mat4(modelview_stack.top());
    modelview_stack.pop();

    addPathPoint();

    for (const auto& child : _children)
    {
        child->updatePath(elapsedSimulatedDays, _modelViewMatrix);
    }
}

void Planet::addPathPoint(){
    _path->addPosition(glm::vec3(_modelViewMatrix * glm::vec4(0,0,0,1)));
}

void Planet::createPath(){
    _path->recreate();
    for(auto child : _children)
        child->createPath();
}

void Planet::setCloudTexture(std::string textureLocation)
{
    _cloudTextureLocation = textureLocation;
}