#version 400 core

// Das 'out' (Ausgabe) für die finale Farbe
out vec4 outColor;

// Das 'in' (Eingabe), das wir von orbit.cpp setzen
uniform vec3 uColor;

void main()
{
    // Setze die Ausgabe-Farbe auf die, die wir per Uniform erhalten
    // (Alpha-Wert auf 1.0 für "sichtbar")
    outColor = vec4(uColor, 1.0);
}