#version 330 core
out vec4 FragColor;

// Empfängt die Farbe vom Vertex Shader
in vec3 vColor;

void main()
{
    // Gib einfach die interpolierte Farbe aus
    FragColor = vec4(vColor, 1.0);
}