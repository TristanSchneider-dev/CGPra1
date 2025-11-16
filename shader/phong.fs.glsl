#version 330 core
out vec4 FragColor;

// Inputs vom Vertex Shader
in vec2 vTexCoord;
in vec3 vNormalView;
in vec3 vFragPosView;

// Uniforms (Sonne)
uniform sampler2D uTextureSampler; // Die Planetentextur (Einheit 0)
uniform vec3 uLightPosView;
uniform vec3 uLightColor;

// Uniforms (Wolken)
uniform sampler2D uCloudSampler; // Die Wolkentextur (Einheit 1)
uniform bool uHasClouds;         // Haben wir überhaupt Wolken?
uniform float uTime;             // Zeit für Animation

// --- NEU: Uniforms (Laser) ---
uniform bool uHasLaser;
uniform vec3 uLaserPosView;     // Position der Kegelspitze
uniform vec3 uLaserDirView;     // Richtung, in die der Laser zeigt
uniform vec3 uLaserColor;       // Farbe (Rot)
uniform float uLaserCutoffCos;  // cos(Cutoff-Winkel)


void main()
{
    // 1. Ambiente Beleuchtung (Sonne)
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * uLightColor;

    // 2. Diffuse Beleuchtung (Sonne)
    vec3 norm = normalize(vNormalView);
    vec3 lightDir = normalize(uLightPosView - vFragPosView);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    // 3. Gesamter Lichteffekt (Sonne)
    vec3 lightEffect = (ambient + diffuse);

    // --- NEU: Laserlichtberechnung (Spotlight) ---
    if (uHasLaser)
    {
        // Vektor vom Fragment zur Laser-Position
        vec3 fragToLaser = uLaserPosView - vFragPosView;
        float dist = length(fragToLaser); // Distanz für Abschwächung
        vec3 fragToLaserNorm = fragToLaser / dist;

        // Winkel zwischen Fragment-Vektor und (umgekehrter) Laser-Richtung
        // uLaserDirView zeigt vom Todesstern weg.
        // -uLaserDirView zeigt zum Todesstern hin (Zentrum des Lichtkegels).
        float theta = dot(fragToLaserNorm, -uLaserDirView);

        // Prüfen, ob das Fragment innerhalb des Kegels liegt
        if (theta > uLaserCutoffCos)
        {
            // Wir sind im Lichtkegel.

            // Intensität mit weichem Rand (smoothstep)
            // Wir nehmen einen kleinen Puffer (epsilon), um einen weichen Rand
            // statt einer harten Kante zu erzeugen.
            float epsilon = 0.01; // Entspricht ca. cos(X) - cos(X + 0.5°)
            float outerCos = uLaserCutoffCos - epsilon;
            float intensity = clamp(smoothstep(outerCos, uLaserCutoffCos, theta), 0.0, 1.0);

            // Diffuse Komponente für den Laser (damit der Spot nicht
            // auf der Rückseite des Planeten leuchtet)
            float laserDiff = max(dot(norm, fragToLaserNorm), 0.0);

            // Abschwächung mit der Distanz (Attenuation)
            float attenuation = 1.0 / (1.0 + 0.05 * dist + 0.01 * (dist * dist));

            // Das rote Laserlicht wird addiert
            vec3 laserContribution = uLaserColor * intensity * laserDiff * attenuation;

            lightEffect += laserContribution;
        }
    }
    // --- ENDE NEU ---

    // 4. Texturfarbe (Erde)
    vec3 textureColor = texture(uTextureSampler, vTexCoord).rgb;

    // 5. Standardfarbe ist die beleuchtete Erde
    vec3 finalColor = lightEffect * textureColor;

    // 6. Wenn Wolken vorhanden sind, überlagere sie
    if (uHasClouds)
    {
        // 1. Wolkenkoordinaten animieren (verschieben)
        vec2 cloudTexCoord = vTexCoord;
        cloudTexCoord.x += uTime * 0.02; // Geschwindigkeit anpassen

        // 2. Wolkenfarbe (als Alpha-Maske) auslesen
        float cloudAlpha = texture(uCloudSampler, cloudTexCoord).r;

        // 3. Die Wolken selbst sind weiß und reflektieren das Licht
        vec3 cloudComponent = lightEffect;

        // 4. Mische (mix) zwischen Erde und Wolken
        finalColor = mix(finalColor, cloudComponent, cloudAlpha);
    }

    FragColor = vec4(finalColor, 1.0);
}