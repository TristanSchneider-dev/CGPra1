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
bool Config::show3DOrbits = true;
bool Config::localOrbits = true;

// --- GEÄNDERT ---
// Standardwert 2 Grad (passend zum UI-Slider)
float Config::laserCutoff = 2.0f;