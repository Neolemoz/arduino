#ifndef IMU_H
#define IMU_H

// ===== IMU FUNCTIONS (เดิม) =====
void zeroYaw() {
  Serial1.begin(115200);
  delay(100);
  Serial1.write(0xA5); Serial1.write(0x54); delay(100);
  Serial1.write(0xA5); Serial1.write(0x55); delay(100);
  Serial1.write(0xA5); Serial1.write(0x52); delay(100);
}

bool getIMU() {
  while (Serial1.available()) {
    rxBuf[rxCnt] = Serial1.read();
    // [FIX] เดิม: if (rxCnt == 0 && rxBuf[0] != 0xAA) return false;
    if (rxCnt == 0 && rxBuf[0] != 0xAA) {
      continue;  // ข้าม byte นี้ไป รอจนกว่าจะเจอ 0xAA
    }
    rxCnt++;
    if (rxCnt == 8) {
      rxCnt = 0;
      if (rxBuf[0] == 0xAA && rxBuf[7] == 0x55) {
        pvYaw = (int16_t)(rxBuf[1] << 8 | rxBuf[2]) / 100.f;
        return true;
      }
    }
  }
  return false;
}

#endif // IMU_H
