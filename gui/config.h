#ifndef CONFIG_H
#define CONFIG_H

namespace Config {
    extern float animationSpeed;
    extern bool localRotation;    // Steuert jetzt "Eigendrehung"
    extern bool GlobalRotation; // Steuert jetzt "Globale Orbits" (um die Erde)
    extern bool sunLight;
    extern bool showWireframe;
    extern bool showOrbits;
    extern bool showCoordinateSystem;
    extern bool show3DOrbits;

    // --- NEU HINZUFÜGEN ---
    extern bool localOrbits;      // Steuert "Lokale Orbits" (um die Sonne)
}

#endif // CONFIG_H