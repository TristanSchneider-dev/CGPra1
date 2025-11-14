#version 330 core
out vec4 FragColor;

// Inputs vom Vertex Shader
in vec2 vTexCoord;
in vec3 vNormalView;
in vec3 vFragPosView;

// Uniforms
uniform sampler2D uTextureSampler; // Die Planetentextur (Einheit 0)
uniform vec3 uLightPosView;
uniform vec3 uLightColor;

// NEUE Uniforms
uniform sampler2D uCloudSampler; // Die Wolkentextur (Einheit 1)
uniform bool uHasClouds;         // Haben wir überhaupt Wolken?
uniform float uTime;             // Zeit für Animation

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

    // Gesamter Lichteffekt (Ambiente + Diffus)
    vec3 lightEffect = (ambient + diffuse);

    // Texturfarbe (Erde)
    vec3 textureColor = texture(uTextureSampler, vTexCoord).rgb;

    // Standardfarbe ist die beleuchtete Erde
    vec3 finalColor = lightEffect * textureColor;

    // Wenn Wolken vorhanden sind, überlagere sie
    if (uHasClouds)
    {
        // 1. Wolkenkoordinaten animieren (verschieben)
        vec2 cloudTexCoord = vTexCoord;
        cloudTexCoord.x += uTime * 0.02; // Geschwindigkeit anpassen

        // 2. Wolkenfarbe (als Alpha-Maske) auslesen
        // Wir nehmen an: Wolkentextur ist Graustufen (Schwarz=transparent, Weiß=Wolke)
        // Wir nutzen den Rot-Kanal als Alpha-Wert.
        float cloudAlpha = texture(uCloudSampler, cloudTexCoord).r;

        // 3. Die Wolken selbst sind weiß und reflektieren das Licht
        vec3 cloudComponent = lightEffect; // * vec3(1.0, 1.0, 1.0)

        // 4. Mische (mix) zwischen Erde und Wolken
        // mix(A, B, x) -> A * (1-x) + B * x
        finalColor = mix(finalColor, cloudComponent, cloudAlpha);
    }

    FragColor = vec4(finalColor, 1.0);
}