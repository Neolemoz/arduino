#ifndef PENALTY_ZONE_H
#define PENALTY_ZONE_H

// ===== Penalty zone strategy toggles =====
#ifndef USE_PENALTY_ZONE
#define USE_PENALTY_ZONE 1   // 1=เปิดตรรกะเขตโทษ, 0=ปิด
#endif

// ความไวการเจอเส้นสีขาว (เฉลี่ย L2,R2)
#ifndef PENALTY_WHITE_THR
#define PENALTY_WHITE_THR 2500
#endif

// เวลาหน่วง ก่อนถอย (ห้ามหยุดนิ่งในเขตโทษ)
#ifndef PENALTY_COUNTDOWN_MS
#define PENALTY_COUNTDOWN_MS 3000UL   // ~3 วินาที
#endif

// ความเร็วคุมท่ารอระหว่างนับถอยหลัง (ไม่หยุด)
#ifndef PENALTY_HOLD_SPEED
#define PENALTY_HOLD_SPEED 40
#endif

// พฤติกรรมถอยออกจากเขตโทษ
#ifndef PENALTY_BACK_SPEED
#define PENALTY_BACK_SPEED 60
#endif

#ifndef PENALTY_BACK_MS
#define PENALTY_BACK_MS 700UL
#endif

// ===== Line sensor readers (แก้ให้เข้ากับบอร์ดของเลม่อน) =====
// ถ้าเลม่อนมีฟังก์ชันอ่าน L2/R2 อยู่แล้ว เช่น l2(), r2() → เปลี่ยนมาใช้ที่นี่
#ifndef READ_L2
  // TODO: แก้ให้ตรงกับพินจริงของ L2 (ตัวอย่างใช้ analogRead(PA1))
  #define READ_L2() analogRead(PA1)
#endif
#ifndef READ_R2
  // TODO: แก้ให้ตรงกับพินจริงของ R2 (ตัวอย่างใช้ analogRead(PA2))
  #define READ_R2() analogRead(PA2)
#endif

// ===== Runtime states (non-destructive, ตัวแปรใหม่ล้วน) =====
static bool          penaltyCountdownActive = false;
static unsigned long penaltyCountdownUntil  = 0;

static bool          penaltyBacking         = false;
static unsigned long penaltyBackUntil       = 0;

#endif // PENALTY_ZONE_H
