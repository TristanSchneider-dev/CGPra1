#version 330 core
out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D uTextureSampler;

void main()
{
    // Wir nehmen die Texturfarbe und geben sie als "leuchtend" aus
    // (Keine ambiente oder diffuse Berechnung)
    FragColor = texture(uTextureSampler, vTexCoord);
}