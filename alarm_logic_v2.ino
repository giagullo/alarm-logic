// Versione 2 
// V2.1: beep di segnale attivo /disattivo su sirena Bentel
//       
#include <limits.h>
#define PIN_SAn  3
#define PIN_A    4
#define PIN_Wn   7
#define PIN_OFFn 8
#define PIN_RNGn 9
#define BUZZ 10
#define STANDBY  0
#define ARMED    1

#define WAIT_FOR_CALL 3
#define RINGING 4

#define WAIT_IN_ALARM 6

#define TIME_WAIT_CALL 15000
#define TIME_RING 3*60000

int state ;
unsigned long timerStart, timerEnd;

void setTimer(unsigned long duration) {
  timerStart  = millis();
  timerEnd  = millis() + duration;
}
bool isTimerExpired() {
  if (millis() > timerEnd)
    return true;
  if (millis() <timerStart) {
    // timer overflow
    timerEnd = (timerEnd -timerStart) - (ULONG_MAX - timerStart);
    if ( millis() > timerEnd) 
      return true;
  }
  return false;
}
void arm() {
  tone(BUZZ,1000, 500);
  delay(1000);
  tone(BUZZ,1000, 500);
  delay(1000);  
  tone(BUZZ,1000, 500);
  delay(1000);  
  tone(BUZZ,2000, 2000);
  digitalWrite(PIN_OFFn, HIGH);
  digitalWrite(PIN_Wn, HIGH);

  // tre brevi beep = sistema armato
  digitalWrite(PIN_RNGn, LOW);
  delay(250);
  digitalWrite(PIN_RNGn, HIGH);
  delay(1000);
  digitalWrite(PIN_RNGn, LOW);
  delay(250);
  digitalWrite(PIN_RNGn, HIGH);
  delay(1000);
  digitalWrite(PIN_RNGn, LOW);
  delay(250);
  
  digitalWrite(PIN_RNGn, HIGH);
  Serial.println("System armed");
  return;
}
void disarm() {
  tone(BUZZ,1500, 500);
  delay(1000);
  tone(BUZZ,1000, 2000);
  digitalWrite(PIN_OFFn, LOW);
  digitalWrite(PIN_Wn, HIGH);

  // un beep lungo = sistema disarmato
  digitalWrite(PIN_RNGn, LOW);
  delay(1000);
  
  digitalWrite(PIN_RNGn, HIGH);
  Serial.println("System disarmed");
  return;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing Arduino logic...");
  state = STANDBY;
  pinMode(PIN_SAn, INPUT);
  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_Wn, OUTPUT);
  pinMode(PIN_OFFn, OUTPUT);
  pinMode(PIN_RNGn, OUTPUT);
  
  digitalWrite(PIN_Wn, HIGH);
  digitalWrite(PIN_OFFn, LOW);
  digitalWrite(PIN_RNGn, HIGH);
  Serial.println("Logic initialized...");
}

void loop() {
  
  if ( state == STANDBY ) {
    if ( digitalRead(PIN_SAn) == LOW ) {
      arm();
      state = ARMED;
    }
  }
  if ( state == ARMED ) {
    if (digitalRead(PIN_A) == HIGH) {
      digitalWrite(PIN_Wn, LOW);
      Serial.println("Now waiting for call");
      setTimer(TIME_WAIT_CALL); state = WAIT_FOR_CALL;
    }
    if (digitalRead(PIN_SAn) == HIGH ) {
      disarm();
      state = STANDBY;
    }
  }
  
  if ( state == WAIT_FOR_CALL || state == RINGING || state == WAIT_IN_ALARM ) {
    if (digitalRead(PIN_SAn) == HIGH) {
      disarm();
      Serial.println("Now returing to standby");
      state = STANDBY;
    }
  }
  if ( state == WAIT_FOR_CALL ) {
    if ( isTimerExpired() ) {
      Serial.println("Now ringing");
      digitalWrite(PIN_RNGn, LOW);
      setTimer(TIME_RING); state = RINGING;
    }
  }
  
  if ( state == RINGING ) {
    if ( isTimerExpired() ) {
      digitalWrite(PIN_RNGn, HIGH);
      Serial.println("Now NOT ringing");
        state = WAIT_IN_ALARM;
    }
  }
}
