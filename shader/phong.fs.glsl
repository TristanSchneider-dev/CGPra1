#version 330 core
out vec4 FragColor;

// Inputs vom Vertex Shader
in vec2 vTexCoord;
in vec3 vNormalView;
in vec3 vFragPosView;

// Uniforms (diese musst du später im C++ Code setzen!)
uniform sampler2D uTextureSampler; // Die Planetentextur
uniform vec3 uLightPosView;     // Position der Sonne (im View-Space)
uniform vec3 uLightColor;       // Farbe der Sonne

void main()
{
    // Ambiente Beleuchtung
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * uLightColor;

    // Diffuse Beleuchtung
    vec3 norm = normalize(vNormalView);
    vec3 lightDir = normalize(uLightPosView - vFragPosView);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    // Texturfarbe
    vec3 textureColor = texture(uTextureSampler, vTexCoord).rgb;

    // Endergebnis (Ambiente + Diffus) * Texturfarbe
    vec3 result = (ambient + diffuse) * textureColor;
    FragColor = vec4(result, 1.0);
}