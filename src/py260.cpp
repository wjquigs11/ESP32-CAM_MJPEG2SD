#include "py260.h"

// Provided by esp32-camera; its private header is not shipped with Arduino.
extern "C" int SCCB_Write16(uint8_t slv_addr, uint16_t reg, uint8_t data);

namespace {
// PY260 encodes signed adjustments as 0, +1, -1, +2, -2, ...
int adjustmentRegister(int level) {
  return level > 0 ? level * 2 - 1 : -level * 2;
}

int setBrightness(sensor_t* sensor, int level) {
  if (level < -4 || level > 4) return -1;
  int res = SCCB_Write16(sensor->slv_addr, 0x0122, adjustmentRegister(level));
  if (res == 0) sensor->status.brightness = level;
  return res;
}

int setContrast(sensor_t* sensor, int level) {
  if (level < -3 || level > 3) return -1;
  int res = SCCB_Write16(sensor->slv_addr, 0x0123, adjustmentRegister(level));
  if (res == 0) sensor->status.contrast = level;
  return res;
}

int setSaturation(sensor_t* sensor, int level) {
  if (level < -3 || level > 3) return -1;
  int res = SCCB_Write16(sensor->slv_addr, 0x0124, adjustmentRegister(level));
  if (res == 0) sensor->status.saturation = level;
  return res;
}

int setExposureCompensation(sensor_t* sensor, int level) {
  if (level < -3 || level > 3) return -1;
  // The UI's PY260 "Exposure" slider is compensation, not manual exposure
  // lines. The bundled set_aec_value writes the high byte to BOTH registers.
  return SCCB_Write16(sensor->slv_addr, 0x0125, adjustmentRegister(level));
}

int setAutoGain(sensor_t* sensor, int enable) {
  // PY260: register 0 = automatic gain/exposure, 1 = manual.
  int res = SCCB_Write16(sensor->slv_addr, 0x0130, enable ? 0 : 1);
  if (res == 0) sensor->status.aec2 = enable != 0;
  return res;
}

int setQuality(sensor_t* sensor, int quality) {
  if (quality < 0 || quality > 2) return -1;
  int res = SCCB_Write16(sensor->slv_addr, 0x012a, quality);
  if (res == 0) sensor->status.quality = quality;
  return res;
}
} // namespace

void configurePy260Sensor(sensor_t* sensor) {
  if (sensor == nullptr || sensor->id.PID != MEGA_CCM_PID) return;
  sensor->set_brightness = setBrightness;
  sensor->set_contrast = setContrast;
  sensor->set_saturation = setSaturation;
  sensor->set_aec_value = setExposureCompensation;
  sensor->set_aec2 = setAutoGain;
  sensor->set_quality = setQuality;
}
