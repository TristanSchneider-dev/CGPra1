#version 330 core
out vec4 FragColor;

// Inputs vom Vertex Shader
in vec2 vTexCoord;
in vec3 vNormalView;
in vec3 vFragPosView;

// Uniforms
uniform sampler2D uTextureSampler; // Die Ringtextur (Einheit 0)
uniform vec3 uLightPosView;
uniform vec3 uLightColor;

void main()
{
    // Texturfarbe (Ring)
    vec4 textureColor = texture(uTextureSampler, vTexCoord);

    // Wir nehmen an, dass die .bmp-Textur keinen Alpha-Kanal hat.
    // Wir verwenden den Rot-Kanal als Alpha-Wert (Graustufen-Maske).
    // Schwarz (r=0) wird komplett transparent, Weiß (r=1) komplett opak.
    float alpha = textureColor.r;

    // Ambiente Beleuchtung
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * uLightColor;

    // Diffuse Beleuchtung
    vec3 norm = normalize(vNormalView);
    vec3 lightDir = normalize(uLightPosView - vFragPosView);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    // Gesamter Lichteffekt (Ambiente + Diffus)
    vec3 lightEffect = (ambient + diffuse);

    // Endgültige Farbe ist die beleuchtete Texturfarbe
    vec3 finalColor = lightEffect * textureColor.rgb;

    // Gib die Farbe mit dem berechneten Alpha-Wert aus
    FragColor = vec4(finalColor, alpha);
}