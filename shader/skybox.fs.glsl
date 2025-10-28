#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

// Wichtig: samplerCube, nicht sampler2D
uniform samplerCube skybox;

void main()
{
    // Liest von der Cubemap-Textur
    FragColor = texture(skybox, TexCoords);
}