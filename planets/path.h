#ifndef PATH_H
#define PATH_H

#include "planets/drawable.h" // Geändert von orbit.h zu drawable.h
#include <vector>             // Hinzugefügt
#include <glm/vec3.hpp>       // Hinzugefügt

/**
 * @brief The Path class describes the absolute path a planet follows
 */
class Path : public Drawable
{
public:
    /**
     * @brief Path constructor
     * @param name the name of the path
     */
    Path(std::string name = "UNKNOWN PATH");

    /**
     * @see Drawable::draw(glm::mat4)
     */
    virtual void draw(glm::mat4 projection_matrix) const override;

    /**
     * @see Drawable::createObject()
     */
    virtual void createObject() override;

    /**
     * @see Drawable::update(float, glm::mat4)
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    /**
     * @brief addPosition Adds a position to the path
     * @param position The position to add
     */
    virtual void addPosition(glm::vec3 position);

protected:

    /**
     * @see Drawable::getVertexShader()
     */
    virtual std::string getVertexShader() const override;

    /**
     * @see Drawable::getFragmentShader()
     */
    virtual std::string getFragmentShader() const override;

    // NEUE MEMBER-VARIABLEN
    std::vector<glm::vec3> _positions; /**< Speichert alle Punkte des Pfades */
    unsigned int _vertexCount = 0;     /**< Anzahl der Vertices, die gezeichnet werden sollen */
};

#endif // PATH_H