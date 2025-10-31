#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// Uniforms
uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

// Outputs für den Fragment Shader
out vec2 vTexCoord;
out vec3 vNormalView;   // Normale im View-Space
out vec3 vFragPosView;  // Position im View-Space

void main()
{
    gl_Position = projection_matrix * modelview_matrix * vec4(aPos, 1.0);

    // Transformiere Position und Normale in den View-Space für die Beleuchtung
    vFragPosView = vec3(modelview_matrix * vec4(aPos, 1.0));
    // (Normale korrekt transformieren, falls Skalierung ungleichmäßig ist)
    vNormalView = mat3(transpose(inverse(modelview_matrix))) * aNormal;

    vTexCoord = aTexCoord;
}