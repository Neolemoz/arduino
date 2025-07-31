#ifndef HUSKY_DETECT_H
#define HUSKY_DETECT_H

// ===== HUSKYLENS BALL DETECTION (เดิม) =====
// [ADD] กันซ้ำเมื่อรวมหลายไฟล์/section
#ifndef HUSKY_INSTANCE_DEFINED
#define HUSKY_INSTANCE_DEFINED
HUSKYLENS husky;
#endif

bool detectBall(int &x, int &w) {
  if (!husky.request()) return false;
  if (!husky.available()) return false;
  HUSKYLENSResult r = husky.read();
  if (r.ID == 1) {
    x = r.xCenter;
    w = r.width;
    return true;
  }
  return false;
}

// [ADD] ตรวจจับวัตถุตาม ID (2=blue, 3=yellow)
bool detectID(uint8_t targetID, int &x, int &w) {
  if (!husky.request())   return false;
  if (!husky.available()) return false;
  while (husky.available()) {
    HUSKYLENSResult r = husky.read();
    if (r.ID == targetID) { x = r.xCenter; w = r.width; return true; }
  }
  return false;
}

#endif // HUSKY_DETECT_H
