#ifndef CONFIG_H
#define CONFIG_H

/*
 * Diese Datei (oder ein Namespace) deklariert globale Konfigurationsvariablen,
 * die von der GUI (MainWindow) gesetzt und von der Logik (Planet, Orbit) gelesen werden.
 */

namespace Config {
    // Vorhandene Variablen (basierend auf deinen Dateien)
    extern float animationSpeed;
    extern bool localRotation;
    extern bool GlobalRotation; // In mainwindow.cpp so genannt
    extern bool sunLight;
    extern bool showWireframe;
    extern bool showOrbits;
    extern bool showCoordinateSystem;

    // --- NEU HINZUGEFÜGT ---
    extern bool show3DOrbits;
}

#endif // CONFIG_H