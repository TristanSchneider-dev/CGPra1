#version 330 core
out vec4 FragColor;

void main()
{
    // Gib eine rote Farbe mit 30% Deckkraft aus.
    // Dies wird als leuchtender Strahl gerendert.
    FragColor = vec4(1.0, 0.0, 0.0, 0.3);
}