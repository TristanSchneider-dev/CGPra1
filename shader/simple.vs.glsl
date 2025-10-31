#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // Muss deklariert werden, auch wenn ungenutzt
layout (location = 2) in vec2 aTexCoord; // Muss deklariert werden, auch wenn ungenutzt

// Uniforms
uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

// Farbe an den Fragment-Shader weitergeben
out vec3 vColor;

void main()
{
    gl_Position = projection_matrix * modelview_matrix * vec4(aPos, 1.0);
    // Benutze die Position als einfache Farbe (ergibt den bunten Look)
    vColor = aPos * 0.5 + 0.5;
}