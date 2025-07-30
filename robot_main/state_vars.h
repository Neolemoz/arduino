#ifndef STATE_VARS_H
#define STATE_VARS_H

// ===== Motor monitor line =====
int m1_cmd = 0, m2_cmd = 0, m3_cmd = 0;
const uint8_t OLED_MOTOR_ROW = 7; // แถวล่างสุดของจอ (ถ้าจอมี 6 แถวให้เปลี่ยนเป็น 5 หรือ 6 ตามจริง)

// ===== MODE DEFINITIONS =====
const int MODE_IDLE = 0;
const int MODE_A    = 1;
const int MODE_B    = 2;

int mode = MODE_IDLE;
bool headingLocked = false;
bool gotBall = false;
bool ball_in_direction = false;

// ===== IMU VARIABLES =====
float pvYaw = 0.0f;
float targetYaw = 0.0f;
uint8_t rxCnt = 0, rxBuf[8];

// ====== (เพิ่ม) ตัวช่วยกันบอลหาย ======
unsigned long lastBallSeen = 0;
const unsigned long BALL_LOST_MS = 400;   // 400–1000 ms ตามจริง

// ====== (เพิ่ม) พารามิเตอร์โค้งเข้าทิศประตู ======
const float    E_DEADBAND   = 2.0f;    // [deg] เล็กกว่านี้ให้วิ่งตรง
const float    K_MS_PER_DEG = 7.0f;    // ms ต่อหนึ่งองศา (เริ่ม 6–8 แล้วจูน)
const uint16_t PULSE_MIN    = 90;      // ms ขั้นต่ำของการโค้งหนึ่งพัลส์
const uint16_t PULSE_MAX    = 350;     // ms สูงสุด (กันโค้งยาวเกิน)
unsigned long  steerUntil   = 0;       // เวลาสิ้นสุดพัลส์โค้งปัจจุบัน
int            steerDir     = 0;       // -1=ซ้าย, +1=ขวา, 0=ตรง

// ====== (เพิ่ม) ใช้ centeredCount ให้มีผลจริง ======
const int CENTER_STABLE_N = 2;         // ต้องอยู่กลางเฟรมติดกัน N รอบก่อนเดินหน้า

// ===== LOOP =====
int centeredCount = 0;   // อยู่ตรงนี้ตามเดิม

#endif // STATE_VARS_H
