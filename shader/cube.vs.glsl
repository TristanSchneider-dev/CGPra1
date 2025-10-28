#version 330 core
layout (location = 0) in vec3 aPos;

// Uniforms für die Matrizen aus planet.cpp
uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

// Farbe an den Fragment-Shader weitergeben
out vec3 vColor;

void main()
{
    // Normale 3D-Transformation (kein .xyww Trick!)
    gl_Position = projection_matrix * modelview_matrix * vec4(aPos, 1.0);

    // Benutze die Position als einfache Farbe (ergibt den bunten Look)
    vColor = aPos * 0.5 + 0.5;
}