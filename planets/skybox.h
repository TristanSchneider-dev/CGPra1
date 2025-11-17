#ifndef SKYBOX_H
#define SKYBOX_H

#include "planets/drawable.h"

class Skybox : public Drawable
{
public:
    Skybox(std::string name = "SKYBOX");

    virtual void init() override;

    virtual void draw(glm::mat4 projection_matrix) const override;

    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

protected:

    virtual std::string getVertexShader() const override;

    virtual std::string getFragmentShader() const override;

    virtual void createObject() override;

    virtual void loadTexture();
};

#endif // SKYBOX_H