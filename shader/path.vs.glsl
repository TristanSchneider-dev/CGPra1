#version 330 core
layout (location = 0) in vec3 aPos; // Erwartet NUR Position

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

void main()
{
    // Die Vertices sind bereits in Welt-Koordinaten,
    // modelview_matrix ist die Identitätsmatrix.
    gl_Position = projection_matrix * modelview_matrix * vec4(aPos, 1.0);
}