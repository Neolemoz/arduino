#ifndef MOTOR_DISPLAY_H
#define MOTOR_DISPLAY_H

// ใช้ตัวแปรจาก state_vars.h

inline void OLED_MotorLine() {
  oled.text(OLED_MOTOR_ROW, 0, "m1=%4d m2=%4d m3=%4d", m1_cmd, m2_cmd, m3_cmd);

  // [FIX-NONDESTRUCTIVE] ย้ายแสดง m3 ไปอีกแถวเพื่อไม่ทับบรรทัดแรก
  uint8_t row2 = (OLED_MOTOR_ROW > 0) ? (OLED_MOTOR_ROW - 1) : OLED_MOTOR_ROW;
  oled.text(row2, 0, "m3=%4d       ", m3_cmd);

  oled.show();
}

// ===== Motion Functions (เดิม) =====
void stop() {
  motor(1, 0);
  motor(2, 0);
  motor(3, 0);
  m1_cmd = 0; m2_cmd = 0; m3_cmd = 0;
  OLED_MotorLine();
}
void forward(int speed) {
  motor(1,  speed);
  motor(2, -speed);
  motor(3,  0);
   m1_cmd =  speed; m2_cmd = -speed; m3_cmd = 0;
  OLED_MotorLine();
}
void backward(int speed) {
  motor(1, -speed);
  motor(2,  speed);
  motor(3,  0);
  m1_cmd = -speed; m2_cmd =  speed; m3_cmd = 0;
  OLED_MotorLine();
}
void cw(int speed) {
  motor(1,  speed);
  motor(2,  speed);
  motor(3,  speed);
  m1_cmd = speed; m2_cmd = speed; m3_cmd = speed;
  OLED_MotorLine();
}
void ccw(int speed) {
  motor(1, -speed);
  motor(2, -speed);
  motor(3, -speed);
  m1_cmd = -speed; m2_cmd = -speed; m3_cmd = -speed;
  OLED_MotorLine();
}

void L_curve(int speedFast, int speedSlow) {
  motor(1, speedFast);   // ล้อหน้า (ซ้ายหน้า)
  motor(2, -speedSlow);  // ล้อหน้า (ขวาหน้า)
  motor(3, 30);          // ล้อหลัง
  oled.text(5, 0, "L_curve");
  oled.show();
   m1_cmd = speedFast; m2_cmd = -speedSlow; m3_cmd = 30;
  OLED_MotorLine();
}

void R_curve(int speedFast, int speedSlow) {
  motor(1, speedSlow);   // ล้อหน้า (ซ้ายหน้า)
  motor(2, -speedFast);  // ล้อหน้า (ขวาหน้า)
  motor(3, -30);         // ล้อหลัง
  oled.text(5, 0, "R_curve");
  oled.show();
  m1_cmd = speedSlow; m2_cmd = -speedFast; m3_cmd = -30;
  OLED_MotorLine();
}

void goal(){
  oled.text(5, 0, "GOALLLL");
  oled.show();
  forward(50);
}

#endif // MOTOR_DISPLAY_H
