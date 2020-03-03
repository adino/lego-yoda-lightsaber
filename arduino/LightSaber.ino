const int G_NOT_PIN = 9;
const int DATA_PIN = 3;
const int CLK_PIN = 4;
const int BTN_PIN = 11;
const int LED = 13;

void set_leds(byte val) {
  shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, 0);
  shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, val & 0xff);
  digitalWrite(CLK_PIN, HIGH);
  delay(1);
  digitalWrite(CLK_PIN, LOW);
}

void gauge(byte leds) {
  int val = 0;
  if (leds > 8) {
    return;
  }
  val = (1 << (leds+1))-1;
  set_leds(val);
}

void setup() {
  pinMode(G_NOT_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  digitalWrite(G_NOT_PIN, HIGH);
  delay(1000);
}

void turn_on() {
  digitalWrite(G_NOT_PIN, LOW);
  for (int i=1; i<9; i++){
    gauge(i);
    delay(25);
  }
}

void turn_off() {
  for (int i=8; i>=0; i--){
    gauge(i);
    delay(10);
  }
  digitalWrite(G_NOT_PIN, HIGH);
}

/* LS stays on */
void flicker(int millis) {
  const int blink_ms=50;
  const int max_blinks=30;
  int step = millis/max_blinks;
  for(; millis>0; millis -=step){
    if (random(millis) <= step) {
      digitalWrite(G_NOT_PIN, HIGH);
      delay(blink_ms);
      digitalWrite(G_NOT_PIN, LOW);
    }else{
       delay(step);
    }
  }
}

/* every other patter, stays on 
 - ms is how long to keep zig-zagging
 - cycle is the length of 1/2 cycle in ms
 */
void zig_zag(int ms, int cycle) {
  digitalWrite(G_NOT_PIN, LOW);
  while(ms>0) {
    set_leds(0b01010101);
    delay(cycle);
    set_leds(0b10101010);
    delay(cycle);
    ms -= 2*cycle;
  }
  set_leds(0xff);
}

/* beam up/down - wrap around, turns off
  - ms is how long to keep doing this 
  - cycle is how long the step is
  - up with >1 , down otherwise
 */
void beam(int ms, int cycle, int up_down) {
  digitalWrite(G_NOT_PIN, LOW);
  int val = 1;
  while(ms>0){
     delay(cycle);
     set_leds((val>>1) & 0xff);
     if (up_down > 0) {
        val = val << 1;
        if (val == 512) val = 1;
     }else{
        if (val == 1) val = 512;
        val = val >> 1;
     }
     ms -= cycle;
  }
  digitalWrite(G_NOT_PIN, HIGH);
}

/* classic K.I.T.T, turns off
 *  - ms is total time
 *  - cycle is the duty cycle
 */
void kitt(int ms, int cycle){
  int beam_time = 8*cycle;
  while(ms>=beam_time){
    beam(beam_time, cycle, 1);
    beam(beam_time, cycle, -1);
    ms -= 2*beam_time;
  }
}

/* start filling in the middlle, turns off
 *  - ms is total time
 *  - cycle is the duty cycle
 */
void mid_fill(int ms, int cycle){
  int v = 0x08;
  digitalWrite(G_NOT_PIN, LOW);
  while (ms>0) {
    if (v==0xff) v = 0x08;
    set_leds(v&0xff);
    delay(cycle);

    v |= v << 1;
    set_leds(v&0xff);
    delay(cycle);

    v |= v >> 1;
    set_leds(v&0xff);
    delay(cycle);
    ms -= (4*cycle);
  } 
  digitalWrite(G_NOT_PIN, HIGH);
}

void loop() {
  int choice = random(20);
  int on_time = 10000;
  switch(choice) {
    case 0:
      beam(3000, 50, 1);
      beam(3000, 50, -1);
      turn_off();
      break;
    case 1:
      beam(3000, 50, -1);
      turn_off();
      break;
    case 2:
      zig_zag(5000, 100);
      turn_off();
      break;
    case 3:
      mid_fill(5000, 50);
      turn_off();
      break;
    case 4:  
     kitt(5000, 60);
     turn_off();
     break;
    default:
     break;
  }
  turn_on();
  while(on_time>0){
    if(digitalRead(BTN_PIN) == LOW) {
        turn_off();
        delay(1000);
        return; 
    }
    delay(10);
    on_time -= 10; 
  }
  flicker(5000);
  turn_off();
  while(digitalRead(BTN_PIN) == HIGH) delay(10);
}
