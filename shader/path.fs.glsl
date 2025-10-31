#version 330 core
out vec4 FragColor;

// Wir geben dem Pfad eine feste Farbe
uniform vec4 uColor;

void main()
{
    FragColor = uColor;
}