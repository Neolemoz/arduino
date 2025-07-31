#ifndef GOAL_PARAMS_H
#define GOAL_PARAMS_H

// ===== [ADD - goal shooting params] =====
#ifndef GOAL_PARAMS_DEFINED
#define GOAL_PARAMS_DEFINED

const int GOAL_X_LEFT   = 120;       // โกลอยู่กลางพอ (ซ้าย)
const int GOAL_X_RIGHT  = 200;       // โกลอยู่กลางพอ (ขวา)
const int GOAL_W_NEAR   = 70;        // ต้องใกล้พอ (กว้างพอ) ถึงเริ่มยิง
const int GOAL_PUSH_SPEED = 70;      // ความเร็วพุ่งยิง
const unsigned long GOAL_PUSH_MS = 1200; // เวลาพุ่งยิง (ms)

bool shooting = false;               // อยู่ในช่วงยิงอยู่หรือไม่
unsigned long shootUntil = 0;        // หมดเวลาพุ่งยิงเมื่อไหร่

#endif // GOAL_PARAMS_DEFINED

// ---------- [ADD-L] Goal detect window & push parameters ----------
// [FIX] ใช้ guard เดียวกันทั้งชุด เพื่อกันซ้ำกับที่ส่วนอื่นอาจประกาศเป็น const มาก่อน
#ifndef GOAL_PARAMS_DEFINED
#define GOAL_PARAMS_DEFINED
  const int GOAL_X_LEFT      = 120;   // กรอบกลางซ้ายของประตูบนภาพ
  const int GOAL_X_RIGHT     = 200;   // กรอบกลางขวา
  const int GOAL_W_NEAR      = 70;    // ต้องใกล้พอ (กว้างพอ) ถึงเริ่มยิง
  const int GOAL_PUSH_SPEED  = 70;    // ความเร็วพุ่งดันเข้าโกล
#endif

#ifndef GOAL_PUSH_MS
#endif

// ---------- [ADD-L] Linear timing Tpush = clamp(A - B*gw0, Tmin, Tmax) ----------
#ifndef L_TIMING_CONSTS
  #define L_TIMING_CONSTS
  const int L_A    = 1400;   // ms (intercept)
  const int L_B    = 8;      // ms ต่อ 1 px ของ gw0
  const int L_TMIN = 450;    // ms ต่ำสุด
  const int L_TMAX = 1100;   // ms สูงสุด
#endif

// ---------- [ADD-L] Shooting/retreat states ----------
#ifndef SHOOT_RET_VARS
  #define SHOOT_RET_VARS
  int          gw0_at_shoot  = 0;      // gw ตอนเริ่มยิง
  unsigned long tpush_ms     = 0;      // เวลาพุ่งที่คำนวณได้

  bool         retreating    = false;
  unsigned long retreatUntil = 0;
  const  int   RETREAT_SPEED = 60;     // ความเร็วถอยหลังกันฟาวล์
  const  unsigned long RETREAT_MS = 700; // เวลาถอยหลังกันฟาวล์
#endif

#endif // GOAL_PARAMS_H
