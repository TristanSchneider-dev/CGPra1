#version 400 core

// Nimmt nur die Position entgegen (location = 0),
// da wir Normals und TexCoords für eine simple Farbe nicht brauchen.
layout (location = 0) in vec3 aPosition;

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

void main()
{
    // Berechne die Position und gib sie weiter
    gl_Position = projection_matrix * modelview_matrix * vec4(aPosition, 1.0);
}