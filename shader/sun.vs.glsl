#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

// Wir geben die Texturkoordinaten einfach weiter
out vec2 vTexCoord;

void main()
{
    gl_Position = projection_matrix * modelview_matrix * vec4(aPos, 1.0);
    vTexCoord = aTexCoord;
}