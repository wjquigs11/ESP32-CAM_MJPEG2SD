#pragma once

#include <sensor.h>

// Adapt the bundled MEGA_CCM driver to the signed controls used by the web UI.
void configurePy260Sensor(sensor_t* sensor);
