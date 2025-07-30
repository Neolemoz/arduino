#ifndef MODE_SELECT_H
#define MODE_SELECT_H

// ===== MODE SELECTION (เดิม) =====
int selectMode() {
  oled.clear();
  oled.text(0, 0, "SELECT MODE:");
  oled.text(1, 0, "Hold A = MODE_A");
  oled.text(2, 0, "Hold B = MODE_B");
  oled.text(3, 0, "None  = IDLE");
  oled.text(5, 0, "Press OK to confirm");
  oled.show();

  bool a = false, b = false;
  bool beepedA = false, beepedB = false;

  while (!SW_OK()) {
    if (SW_A() && !a) {
      a = true;
      if (!beepedA) { beep(); beepedA = true; }
    }
    if (SW_B() && !b) {
      b = true;
      if (!beepedB) { beep(); beepedB = true; }
    }
    delay(10);
  }
  while (SW_OK());

  int m = MODE_IDLE;
  if (a) m = MODE_A;
  else if (b) m = MODE_B;

  oled.clear();
  oled.text(0, 0, "MODE %d SELECTED", m);
  oled.text(2, 0, "Heading next...");
  oled.show();
  delay(800);
  return m;
}

#endif // MODE_SELECT_H
