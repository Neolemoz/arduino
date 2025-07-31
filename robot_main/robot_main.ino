#include <POP32.h>
#include <HUSKYLENS.h>
#include <Wire.h>
#include <math.h>

#include "state_vars.h"
#include "motor_display.h"
#include "imu.h"
#include "husky_detect.h"
#include "mode_select.h"
#include "goal_params.h"
#include "penalty_zone.h"

#define setup helpers__setup_from_header
#define loop  helpers__loop_from_header
#include "helpers.h"
    // ===== Strategy toggles (non-destructive) =====
    // ปิดเงื่อนไข "กว้างพอ" ของโกล (gw >= GOAL_W_NEAR)
    #ifndef REQUIRE_GOAL_WIDTH_NEAR
    #define REQUIRE_GOAL_WIDTH_NEAR 0
    #endif

    // ปิดทั้งบล็อก SHOOT Method L (คำนวณเวลาพุ่งแบบเชิงเส้น)
    #ifndef USE_METHOD_L
    #define USE_METHOD_L 0
    #endif

    // ปิดพฤติกรรม "ถอยหลังหลังยิง"
    #ifndef USE_RETREAT_AFTER_SHOOT
    #define USE_RETREAT_AFTER_SHOOT 0
    #endif

#undef setup
#undef loop

// ===== SETUP (เดิม) =====
void setup() {
  oled.begin();
  Wire.begin();
  husky.begin(Wire);
  Serial1.begin(115200);

  zeroYaw();
  mode = selectMode();

  oled.clear();
  oled.text(0, 0, "SET HEADING");
  oled.text(2, 0, "Yaw=--.-");
  oled.text(4, 0, "Press OK...");
  oled.show();

  while (!headingLocked) {
    if (getIMU()) {
      oled.text(2, 0, "Yaw=%.1f   ", pvYaw);
      oled.show();
    }
    if (SW_OK()) {
      targetYaw = pvYaw;
      headingLocked = true;
      while (SW_OK());
    }
    delay(10);
  }

  oled.clear();
  oled.text(2, 0, "START!");
  oled.show();
  beep();
  delay(500);
}

void loop() {
  getIMU();
  float diff = targetYaw - pvYaw;
  if (diff > 180) diff -= 360;
  if (diff < -180) diff += 360;

  int ballX = -1;
  int ballW = 0;
  bool seen = detectBall(ballX, ballW);
  if (seen) lastBallSeen = millis();

  // ===== Step 1 : finding the ball and get it to direction (เดิม + กันค่าค้าง + ใช้ centeredCount) =====
  while(!ball_in_direction){
    while(!gotBall){
      seen = detectBall(ballX, ballW);

      // กันค่าค้างเมื่อไม่เห็นบอล
      if(!seen){                 // ADDED: กันค่าค้างเมื่อบอลหลุดเฟรม
        ballX = -1;
        ballW = 0;
        // [FIX] เดิมเป็น stop(); → ให้หมุนหาแทน
        ccw(25);                 // หรือ cw(25); ถ้าหุ่นหมุนทางนี้หาบอลง่ายกว่า
        centeredCount = 0;       // กันนับกลางเฟรมค้าง
        oled.text(5, 0, "Searching...");
        oled.show();
        continue;                // ไปอ่านเฟรมถัดไป
      } else {
        lastBallSeen = millis();
      }

      if (ballX < 100) {        // ซ้าย → เข้าหาบอลทางซ้าย
        ccw(25);
        centeredCount = 0;      // รีเซ็ตตัวนับเมื่อไม่กลาง
        oled.text(5, 0, "ccw !");
        oled.show();
      } else if (ballX > 220) { // ขวา → เข้าหาบอลทางขวา
        cw(25);
        centeredCount = 0;      // รีเซ็ตตัวนับเมื่อไม่กลาง
        oled.text(5, 0, "cw !");
        oled.show();
      } else {
        // อยู่กลางเฟรม → ต้องอยู่ติดกัน N รอบก่อนเดินหน้า
        centeredCount++;
        if (centeredCount < CENTER_STABLE_N) {
          stop();
          oled.text(5, 0, "Centering %d/%d", centeredCount, CENTER_STABLE_N);
          oled.show();
          continue;             // ยังไม่ forward จนกว่าจะครบ N
        }

        forward(40);
        if(ballW > 45) {
          gotBall = true;
          centeredCount = 0;    // รีเซ็ตหลังได้บอล
          oled.text(1, 0, "Got Ball !");
          oled.show();
        } else {
          oled.text(5, 0, "X: %d W: %d", ballX, ballW);
          oled.show();
        }
      }
    } // return when got ball

    // ===== Step 2 : ครองบอลแล้ว → โค้งเข้าทิศประตูตาม IMU =====

    // กันบอลหายค้างสถานะ gotBall
    seen = detectBall(ballX, ballW);
    if (seen) {
      lastBallSeen = millis();
    } else if (millis() - lastBallSeen > BALL_LOST_MS) {
      gotBall = false;                   // บอลหายจริง → กลับไปหาใหม่
      centeredCount = 0;
      stop();
      oled.text(1, 0, "Ball Lost       ");
      oled.show();
      continue; // กลับไปเข้า while(!gotBall) โดยอัตโนมัติ
    }

    // อัปเดต IMU + แสดงผล
    getIMU();
    diff = targetYaw - pvYaw;
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;

    oled.text(0, 0, "Yaw: %.1f   ", pvYaw);
    oled.text(1, 0, "Err: %f", diff);
    oled.text(2, 0, seen ? "Ball: YES" : "Ball: NO ");
    oled.text(3, 0, gotBall ? "Got: YES " : "Got: NO  ");
    oled.text(4, 0, "X: %d W: %d", ballX, ballW);
    oled.show();


    // [ADD - Pre-shoot: ตรวจสีโกลตามโหมด แล้วพุ่งดันเมื่อโกลอยู่กลางและใกล้พอ]
  uint8_t goalID = (mode == MODE_A) ? 2 : (mode == MODE_B) ? 3 : 0;
  if (goalID != 0) { // โหมด A/B เท่านั้น; IDLE ไม่ทำอะไร
  int gx = -1, gw = 0;
  bool seeGoal = detectID(goalID, gx, gw);  // หรือ detectBall(gx,gw,goalID); ถ้าใช้ overload

  // ถ้าอยู่ในช่วง "กำลังยิง" → พุ่งต่อจนหมดเวลา (ตัด curve ออกชั่วคราว)
  if (shooting) {
    forward(GOAL_PUSH_SPEED);
    if (millis() >= shootUntil) {
      shooting = false;
      // ต้องการหยุดหลังยิงหรือไม่? ถ้าใช่: stop();
      // ต้องการออกจาก while หลักหรือไม่? ถ้าใช่: ball_in_direction = true;
    }
    oled.text(6, 0, "PUSH gx=%d gw=%d", gx, gw);
    oled.show();
    continue; // ขณะกำลังยิง ข้ามการ curve ชั่วคราว
  }

  // ยังไม่เริ่มยิง: ต้องเห็นโกลสีถูก + อยู่กลางพอ + ใกล้พอ (gw >= GOAL_W_NEAR)  *** อันนี้ไม่ใช้ก็เลยบลอคไปแล้ว ***
  if (seeGoal && gx >= GOAL_X_LEFT && gx <= GOAL_X_RIGHT &&
    (REQUIRE_GOAL_WIDTH_NEAR ? (gw >= GOAL_W_NEAR) : true)) {

}
// [END ADD]
    // ---------- [ADD] SHOOT (Method L) & RETREAT ----------
#if USE_METHOD_L
{
  // เลือก ID ของโกลตามโหมด: A=2 (blue), B=3 (yellow); idle=0 (ไม่ยิง)
  uint8_t goalID = (mode == MODE_A) ? 2 : (mode == MODE_B) ? 3 : 0;

  if (goalID != 0) {
    int gx = -1, gw = 0;
    bool seeGoal = detectID(goalID, gx, gw);

    // (1) ถ้ากำลังถอยกันฟาวล์ → ถอยต่อจนหมดเวลา (เปิดได้ด้วยแฟล็ก)
    if (USE_RETREAT_AFTER_SHOOT && retreating) {
      backward(RETREAT_SPEED);
      if (millis() >= retreatUntil) {
        retreating = false;
        stop();
      }
      oled.text(6, 0, "RETREAT"); oled.show();
      continue;
    }

    // (2) ถ้ากำลังพุ่งยิงอยู่แล้ว → พุ่งต่อจนถึงเวลา
    if (shooting) {
      forward(GOAL_PUSH_SPEED);
      if (millis() >= shootUntil) {
        shooting = false;
        #if USE_RETREAT_AFTER_SHOOT
          retreating   = true;
          retreatUntil = millis() + RETREAT_MS;
          backward(RETREAT_SPEED);
          oled.text(6, 0, "RETREAT start"); oled.show();
        #else
          stop();
          oled.text(6, 0, "PUSH done"); oled.show();
        #endif
      } else {
        oled.text(6, 0, "PUSH t=%lu", shootUntil - millis()); oled.show();
      }
      continue;
    }

    // (3) เริ่มยิงเมื่อ "สีถูก + อยู่กลาง" (จะเช็ค/ไม่เช็ค gw ตามแฟล็ก)
    if (seeGoal && gx >= GOAL_X_LEFT && gx <= GOAL_X_RIGHT &&
        (REQUIRE_GOAL_WIDTH_NEAR ? (gw >= GOAL_W_NEAR) : true)) {
      gw0_at_shoot = gw;
      long calc = (long)L_A - (long)L_B * (long)gw0_at_shoot;
      if (calc < L_TMIN) calc = L_TMIN;
      if (calc > L_TMAX) calc = L_TMAX;
      tpush_ms = (unsigned long)calc;

      shooting   = true;
      shootUntil = millis() + tpush_ms;
      forward(GOAL_PUSH_SPEED);

      oled.text(6, 0, "SHOOT %s gw0=%d t=%lu",
                (mode==MODE_A?"BLUE":"YEL"), gw0_at_shoot, tpush_ms);
      oled.show();
      continue;
    }
  }
}
#endif // USE_METHOD_L
// ---------- [END ADD] ----------

    // ---------- [ADD-PENALTY] Non-blocking penalty-area behavior ----------
#if USE_PENALTY_ZONE
{
  // ทำงานเฉพาะโหมดยิง (MODE_A/MODE_B)
  if (mode == MODE_A || mode == MODE_B) {

    // ยืนยันว่ากำลังเล็ง "ประตูสีที่ถูกต้อง" อยู่กลางพอ (ไม่ต้องเช็ค gw)
    uint8_t goalID = (mode == MODE_A) ? 2 : (mode == MODE_B) ? 3 : 0;
    int gx = -1, gw = 0;
    bool seeGoal = (goalID != 0) ? detectID(goalID, gx, gw) : false;

    if (seeGoal && gx >= GOAL_X_LEFT && gx <= GOAL_X_RIGHT) {
      // อ่านเส้น L2/R2 แล้วคำนวณค่าเฉลี่ย
      int l2 = READ_L2();
      int r2 = READ_R2();
      int avgWhite = (l2 + r2) / 2;

      // 1) เริ่มนับถอยหลังเมื่อ "เจอเส้นสีขาว" (เฉลี่ยมากกว่า THR) และยังไม่อยู่ในช่วงอื่น
      if (!penaltyCountdownActive && !penaltyBacking && avgWhite > PENALTY_WHITE_THR) {
        penaltyCountdownActive = true;
        penaltyCountdownUntil  = millis() + PENALTY_COUNTDOWN_MS;
        oled.text(6, 0, "PENAL CNT %lus", (PENALTY_COUNTDOWN_MS/1000UL));
        oled.show();
      }

      // 2) ระหว่างรอครบเวลา: หุ่น "ห้ามหยุด" → คุมความเร็วคงที่ (หรือจะคุมให้เบนซ้าย/ขวาก็ได้)
      if (penaltyCountdownActive) {
        if (millis() >= penaltyCountdownUntil) {
          penaltyCountdownActive = false;
          penaltyBacking         = true;
          penaltyBackUntil       = millis() + PENALTY_BACK_MS;

          backward(PENALTY_BACK_SPEED);
          oled.text(6, 0, "PENAL BACK");
          oled.show();
          continue; // เข้ากระบวนถอยแล้ว
        } else {
          // เดินหน้าคงที่ ไม่หยุดนิ่งในเขตโทษ
          forward(PENALTY_HOLD_SPEED);
          unsigned long remain = (penaltyCountdownUntil - millis() + 999) / 1000UL;
          oled.text(6, 0, "CNT %lu", remain);
          oled.show();
          continue; // ระหว่างนับถอยหลัง ตัดตรรกะอื่นชั่วคราว
        }
      }

      // 3) กำลังถอยออกจากเขตโทษ
      if (penaltyBacking) {
        backward(PENALTY_BACK_SPEED);
        if (millis() >= penaltyBackUntil) {
          penaltyBacking = false;
          stop(); // หรือจะให้ไปหา/ประคองบอลต่อก็เปลี่ยนได้
        }
        oled.text(6, 0, "BACK..");
        oled.show();
        continue; // ระหว่างถอย ตัดตรรกะอื่นชั่วคราว
      }
    }
  }
}
#endif
// ---------- [END ADD-PENALTY] ----------

    // === Curve-to-goal (pulse, non-blocking) ===
    float err = normYawError(targetYaw, pvYaw);
    unsigned long now = millis();

    // ในกรอบเดดแบนด์ → วิ่งตรง ประคองบอล
    if (fabs(err) <= E_DEADBAND) {
      forward(50);
      steerDir   = 0;
      steerUntil = 0;
    } else {
      // ถ้าหมดพัลส์เก่าแล้ว ค่อยออกคำสั่งโค้งรอบใหม่ตาม err ปัจจุบัน
      if (now >= steerUntil) {
        uint16_t ms = curvePulseMs(err);

        if (err > 0) {
          // ต้องเพิ่ม yaw ไปทางขวา ⇒ โค้งขวา
          R_curve(50, 25);
          steerDir   = +1;
          steerUntil = now + ms;
          oled.text(6, 0, "Curve R %3u ms", ms);
        } else {
          // ต้องเพิ่ม yaw ไปทางซ้าย ⇒ โค้งซ้าย
          L_curve(50, 25);
          steerDir   = -1;
          steerUntil = now + ms;
          oled.text(6, 0, "Curve L %3u ms", ms);
        }
        oled.show();
      }
      // ถ้ายังไม่ถึง steerUntil: ปล่อยให้คำสั่งโค้งเดิมทำงานต่อเอง
    }
    // === END Curve-to-goal ===
    } // while(!ball_in_direction)
  }
}