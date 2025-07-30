void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
#ifndef HELPERS_H
#define HELPERS_H

// [ADD - helpers]
float normYawError(float tgt, float pv){
  float e = tgt - pv;
  if (e > 180.f)  e -= 360.f;
  if (e < -180.f) e += 360.f;
  return e;
}
uint16_t curvePulseMs(float err){
  float mag = fabs(err);
  if (mag > 35.f) mag = 35.f;                 // อิ่มตัวที่ ~35°
  uint16_t ms = (uint16_t)(PULSE_MIN + K_MS_PER_DEG * mag);
  if (ms < PULSE_MIN) ms = PULSE_MIN;
  if (ms > PULSE_MAX) ms = PULSE_MAX;
  return ms;
}

#endif // HELPERS_H
