#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

/* ================= LCD ================= */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ================= DFPLAYER ================= */
SoftwareSerial mp3Serial(8, 9); // RX, TX
DFRobotDFPlayerMini dfplayer;

/* ================= PINOS ================= */
const int BUZZER_PIN = 6;
const int LED_RED = 7;
const int LED_GREEN = 13;

/* ================= KEYPAD ================= */
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;

byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A0};

char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

/* ================= GAME ================= */
const unsigned long PLANT_COUNTDOWN_MS = 40000UL;
const byte CODE_LENGTH = 6;

enum GameState {
  GAME_IDLE,
  GAME_PLANTED,
  GAME_DEFUSING,
  GAME_DEFUSED,
  GAME_EXPLODED,
  GAME_VOLUME,
  GAME_CHANGE_CODE
};

GameState state = GAME_IDLE;

/* ================= VARIÁVEIS ================= */
unsigned long plantTimestamp = 0;
unsigned long lastBeepMs = 0;
bool ledState = false;

String secretCode = "123456";
String entryBuffer = "";

/* ===== ÁUDIO SEQUENCIAL ===== */
bool pendingSecondSound = false;
unsigned long soundTimestamp = 0;
int secondSoundToPlay = 0;

/* ===== VOLUME ===== */
int volumeLevel = 20;

/* ================= FUNÇÕES ================= */

void playPlantSound() {
  dfplayer.play(1);
  tone(BUZZER_PIN, 1200, 100);
}

void scheduleSecondSound(int sound, unsigned long delayMs = 2500) {
  pendingSecondSound = true;
  secondSoundToPlay = sound;
  soundTimestamp = millis() + delayMs;
}

void updateSecondSound() {
  if (pendingSecondSound && millis() >= soundTimestamp) {
    dfplayer.play(secondSoundToPlay);
    pendingSecondSound = false;
  }
}

/* ================= BEEP ================= */
void updateBeepAndLed() {
  unsigned long elapsed = millis() - plantTimestamp;
  if (elapsed >= PLANT_COUNTDOWN_MS) return;

  unsigned long remaining = PLANT_COUNTDOWN_MS - elapsed;
  unsigned long interval;

  if (remaining > 15000) interval = 1000;
  else if (remaining > 8000) interval = 500;
  else if (remaining > 3000) interval = 250;
  else interval = 120;

  if (millis() - lastBeepMs >= interval) {
    lastBeepMs = millis();
    ledState = !ledState;
    digitalWrite(LED_RED, ledState);
    tone(BUZZER_PIN, 1000, 60);
  }
}

/* ================= TELAS ================= */
void showIdle() {
  lcd.clear();
  lcd.print("A=Plant D=Vol");
  lcd.setCursor(0,1);
  lcd.print("Vol: ");
  lcd.print(volumeLevel);
}

void showCountdown() {
  unsigned long remaining =
    (PLANT_COUNTDOWN_MS - (millis() - plantTimestamp) + 500) / 1000;

  lcd.setCursor(0,0);
  lcd.print("Time: ");
  if (remaining < 10) lcd.print("0");
  lcd.print(remaining);
  lcd.print("s   ");
}

/* ================= EVENTOS ================= */
void explode() {
  state = GAME_EXPLODED;
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);

  dfplayer.play(3);
  scheduleSecondSound(5);

  lcd.clear();
  lcd.print("!! EXPLODED !!");
  lcd.setCursor(0,1);
  lcd.print("A=Novo C=Cod");
}

void defused() {
  state = GAME_DEFUSED;
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, LOW);

  dfplayer.play(2);
  scheduleSecondSound(4);

  lcd.clear();
  lcd.print("DEFUSED!");
  lcd.setCursor(0,1);
  lcd.print("A=Novo C=Cod");
}

/* ================= SETUP ================= */
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  lcd.init();
  lcd.backlight();

  mp3Serial.begin(9600);
  dfplayer.begin(mp3Serial);
  dfplayer.volume(volumeLevel);

  showIdle();
}

/* ================= LOOP ================= */
void loop() {
  updateSecondSound();
  char k = keypad.getKey();

  switch (state) {

    case GAME_IDLE:
      if (k == 'A') {
        state = GAME_PLANTED;
        plantTimestamp = millis();
        lastBeepMs = 0;
        lcd.clear();
        lcd.print("Plantada");
        playPlantSound();
      }
      if (k == 'D') {
        state = GAME_VOLUME;
        lcd.clear();
        lcd.print("Volume: ");
        lcd.print(volumeLevel);
        lcd.setCursor(0,1);
        lcd.print("A+ B- C=OK");
      }
      break;

    case GAME_PLANTED:
      showCountdown();
      updateBeepAndLed();
      if (k == 'B') {
        state = GAME_DEFUSING;
        entryBuffer = "";
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Dig cod");
        lcd.setCursor(0,1);
        for (int i = 0; i < CODE_LENGTH; i++) lcd.print('_');
        lcd.setCursor(0,1);
      }
      if (millis() - plantTimestamp >= PLANT_COUNTDOWN_MS) explode();
      break;

    case GAME_DEFUSING:
      showCountdown();
      updateBeepAndLed();

      if (k >= '0' && k <= '9' && entryBuffer.length() < CODE_LENGTH) {
        entryBuffer += k;
        lcd.setCursor(entryBuffer.length() - 1, 1);
        lcd.print(k);
      }

      if (entryBuffer.length() == CODE_LENGTH) {
        if (entryBuffer == secretCode) {
          defused();
        } else {
          tone(BUZZER_PIN, 600, 200);
          entryBuffer = "";
          lcd.setCursor(0,1);
          for (int i = 0; i < CODE_LENGTH; i++) lcd.print('_');
          lcd.setCursor(0,1);
        }
      }
      break;

    case GAME_VOLUME:
      if (k == 'A' && volumeLevel < 30) volumeLevel++;
      if (k == 'B' && volumeLevel > 0) volumeLevel--;
      if (k == 'C') {
        dfplayer.volume(volumeLevel);
        state = GAME_IDLE;
        showIdle();
      }
      lcd.setCursor(8,0);
      lcd.print(volumeLevel);
      lcd.print(" ");
      break;

    case GAME_DEFUSED:
    case GAME_EXPLODED:
      if (k == 'A') {
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, LOW);
        state = GAME_IDLE;
        showIdle();
      }
      if (k == 'C') {
        state = GAME_CHANGE_CODE;
        entryBuffer = "";
        lcd.clear();
        lcd.print("Novo Codigo:");
        lcd.setCursor(0,1);
      }
      break;

    case GAME_CHANGE_CODE:
      if (k >= '0' && k <= '9' && entryBuffer.length() < CODE_LENGTH) {
        entryBuffer += k;
        lcd.print(k);
      }
      if (entryBuffer.length() == CODE_LENGTH) {
        secretCode = entryBuffer;
        entryBuffer = "";
        lcd.clear();
        lcd.print("Codigo Salvo!");
        delay(1000);
        state = GAME_IDLE;
        showIdle();
      }
      break;
  }
}
