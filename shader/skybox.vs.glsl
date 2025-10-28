#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view; // Die _modelViewMatrix (ohne Translation)

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);

    // Dieser Trick setzt die Tiefe (z/w) auf 1.0
    gl_Position = pos.xyww;
}