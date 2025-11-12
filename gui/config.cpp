#include "gui/config.h"

/*
 * Hier werden die globalen Variablen initialisiert.
 */

// Initialisierung der vorhandenen Variablen
float Config::animationSpeed = 1.0f; // Standardwert 1.0x (entspricht Slider-Wert 2)
bool Config::localRotation = true;
bool Config::GlobalRotation = true;
bool Config::sunLight = true;
bool Config::showWireframe = false;
bool Config::showOrbits = true;
bool Config::showCoordinateSystem = false;

// --- NEU HINZUGEFÜGT ---
// Standardmäßig die korrekten 3D-Bahnen anzeigen
bool Config::show3DOrbits = true;
bool Config::localOrbits = true;