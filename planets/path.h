#ifndef PATH_H
#define PATH_H

#include "planets/drawable.h"
#include <vector>
#include <glm/vec3.hpp>

class Path : public Drawable
{
public:
    Path(std::string name = "UNKNOWN PATH");

    virtual void draw(glm::mat4 projection_matrix) const override;

    virtual void createObject() override;

    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    virtual void addPosition(glm::vec3 position);

protected:

    virtual std::string getVertexShader() const override;

    virtual std::string getFragmentShader() const override;

    std::vector<glm::vec3> _positions;
    unsigned int _vertexCount = 0;
};

#endif // PATH_H