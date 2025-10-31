#include <GL/glew.h>
#include "planet.h"


#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <stack>
#include <vector> // Hinzugefügt für die Geometrieerstellung

#include "glbase/gltool.hpp"

#include "gui/config.h"

#include "planets/cone.h"
#include "planets/sun.h"
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
    _daysPerYear(daysPerYear)
{
    _localRotationSpeed = 1.0f / hoursPerDay; // for local rotation:one step equals one hour

    _orbit = std::make_shared<Orbit>(name + " Orbit", _distance);
    _path = std::make_shared<Path>(name + " Pfad");

    /// TODO: init global rotation parameters
}

void Planet::init()
{
    Drawable::init();

    /// TODO: load texture

    /// TODO: init children, orbit and path
}

void Planet::recreate()
{
    Drawable::recreate();
    /// TODO: recreate all drawables that belong to this planet
    // Hint: not all drawables need to be recreated
}



void Planet::draw(glm::mat4 projection_matrix) const
{

    /// TODO: replace this with your code

    /// TODO: call draw on all drawables that belong to the planet

    if(_program == 0){
        std::cerr << "Planet" << _name << "not initialized. Call init() first." << std::endl;
        return;
    }

    // Load program
    glUseProgram(_program);

    // bin vertex array object
    glBindVertexArray(_vertexArrayObject);

    // set parameter
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

    // call draw
    // Die feste '36' wurde durch _indexCount ersetzt
    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);

    // unbin vertex array object
    glBindVertexArray(0);

    // check for errors
    VERIFY(CG::checkError());
}

void Planet::update(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    ///TODO: calculate global rotation

    ///TODO: update all drawables that belong to the planet


    // calculate new local rotation
    if(Config::localRotation)
        _localRotation += elapsedTimeMs * _localRotationSpeed * Config::animationSpeed;

    // keep rotation between 0 and 360
    while(_localRotation >= 360.f)
        _localRotation -= 360.0f;
    while(_localRotation < 0.0f)
        _localRotation += 360.0f;

    // apply local rotation to model view matrix
    // Hint: The stack is currently useless, but could be useful for you
    std::stack<glm::mat4> modelview_stack;

    modelview_stack.push(modelViewMatrix);

        // rotate around y-axis
        modelview_stack.top() = glm::rotate(modelview_stack.top(), glm::radians(_localRotation), glm::vec3(0,1,0));
        _modelViewMatrix = glm::mat4(modelview_stack.top());

    modelview_stack.pop();
}

void Planet::setLights(std::shared_ptr<Sun> sun, std::shared_ptr<Cone> laser)
{
    _sun = sun;
    _laser = laser;
    for(auto child : _children)
        child->setLights(sun, laser);
}

void Planet::addChild(std::shared_ptr<Planet> child)
{
    _children.push_back(child);
}


void Planet::createObject(){

    // Auflösung der Kugel (kann angepasst werden)
    // Um die Anforderung "Auflösung pro Objekt angepasst" zu erfüllen,
    // könnte man diese Werte auch als Member der Klasse (im Konstruktor gesetzt) definieren.
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

            // Normale (für Kugel am Ursprung = normalisierter Positionsvektor)
            normals.push_back(glm::normalize(glm::vec3(x, y, z)));

            // Texturkoordinaten
            texCoords.push_back(glm::vec2(u, 1.0f - v)); // 1.0f - v, da Texturen oft oben links starten
        }
    }

    // Indizes für die Dreiecke (Grid aus Quads)
    for (unsigned int i = 0; i < latitudeSegments; ++i)
    {
        for (unsigned int j = 0; j < longitudeSegments; ++j)
        {
            unsigned int v1 = (i * (longitudeSegments + 1)) + j;
            unsigned int v2 = v1 + 1;
            unsigned int v3 = ((i + 1) * (longitudeSegments + 1)) + j;
            unsigned int v4 = v3 + 1;

            // Erstes Dreieck
            indices.push_back(v1);
            indices.push_back(v3);
            indices.push_back(v2);

            // Zweites Dreieck
            indices.push_back(v2);
            indices.push_back(v3);
            indices.push_back(v4);
        }
    }

    // Annahme: _indexCount ist Teil der Basisklasse Drawable
    _indexCount = static_cast<unsigned int>(indices.size());


    // Set up a vertex array object for the geometry
    if(_vertexArrayObject == 0)
        glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    // fill vertex array object with data
    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // layout (location = 0)
    glEnableVertexAttribArray(0);

    // Normalen-Buffer
    GLuint normal_buffer;
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); // layout (location = 1)
    glEnableVertexAttribArray(1);

    // Texturkoordinaten-Buffer
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

    // unbind vertex array object
    glBindVertexArray(0);

    // Buffer können gelöscht werden, da sie im VAO "gespeichert" sind
    // (Die Kommentare im Originalcode wurden beibehalten)
    // glDeleteBuffers(1, &position_buffer);
    // glDeleteBuffers(1, &normal_buffer);
    // glDeleteBuffers(1, &texcoord_buffer);
    // glDeleteBuffers(1, &index_buffer);

    // check for errors
    VERIFY(CG::checkError());
}

std::string Planet::getVertexShader() const
{
    // Diese Shader müssen ggf. angepasst werden, um Normalen (loc 1)
    // und Texturkoordinaten (loc 2) zu verarbeiten.
    return Drawable::loadShaderFile(":/shader/phong.vs.glsl");

}

std::string Planet::getFragmentShader() const
{
    // Diese Shader müssen ggf. angepasst werden, um Normalen
    // und Texturkoordinaten zu verarbeiten (z.B. für Beleuchtung).
    return Drawable::loadShaderFile(":/shader/phong.fs.glsl");
}

Planet::~Planet(){
}


/*************************************************
 * Hint: You don't need to change the code below *
 *************************************************/

void Planet::calculatePath(glm::mat4 modelViewMatrix)
{
    // Hint: if you have some time left you can optimize this method
    for(auto child : _children){
        unsigned int longestCommonMultiple = child->getCommonYears(_daysPerYear);
        for(unsigned int i = 0; i <= longestCommonMultiple; i++){
            float step = 360.0f * _daysPerYear / Config::animationSpeed;
            child->updatePath(step, modelViewMatrix);
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

void Planet::updatePath(float elapsedTimeMs, glm::mat4 modelViewMatrix)
{
    update(elapsedTimeMs, modelViewMatrix);
    addPathPoint();
}

void Planet::addPathPoint(){
    _path->addPosition(glm::vec3(_modelViewMatrix * glm::vec4(0,0,0,1)));
    for(auto child : _children)
        child->addPathPoint();
}

void Planet::createPath(){
    _path->recreate();
    for(auto child : _children)
        child->createPath();
}