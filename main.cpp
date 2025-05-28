#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Definire pini pentru DHT11 si buzzer
#define DHTPIN 3
#define DHTTYPE DHT11
#define BUZZER_PIN 4

// Initializare senzor si LCD
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variabile globale pentru afisaj si masuratori
volatile uint8_t displayState = 0;
float temp = 0.0, hum = 0.0;
unsigned long lastCSVTime = 0;
unsigned long currentTime = 0;
bool nightMode = false;

// Timp de pornire simulat (ora de pe PC)
int startHour = 12;
int startMinute = 59;
unsigned long startMillis = 0;

// Structura pentru note din melodie
struct Note {
  int frequency;
  int duration;
};

// Melodie alerta (frecventa si durata)
Note melody1[] = {
  {660, 200}, {660, 200}, {0, 200}, {660, 200}, {0, 100},
  {520, 200}, {660, 200}, {0, 100}, {780, 200}, {0, 400}
};

int melody1Length = sizeof(melody1) / sizeof(Note);
Note* currentMelody = melody1;
int currentMelodyLength = melody1Length;

// Intrerupere pentru Timer1 - schimba afisajul LCD
ISR(TIMER1_COMPA_vect) {
  displayState = (displayState + 1) % 3;
  if (displayState == 0)
    OCR1A = 62500; // 4 sec pentru afisare mod zi sau noapte
  else
    OCR1A = 46874; // 3 sec pentru temp si umiditate
}

// Configurare Timer1 hardware
void setupTimer1() {
  noInterrupts();           

  TCCR1A = 0;                      // Reseteaza registrul de control A al Timer1
  TCCR1B = 0;                      // Reseteaza registrul de control B al Timer1
  TCNT1 = 0;                       // Reseteaza valoarea contorului Timer1

  OCR1A = 31249;                   // Seteaza valoarea de comparatie pentru 2 secunde
                                  // 16000000 / 1024 = 15625 tick-uri/secunda 
  TCCR1B |= (1 << WGM12);         // Mod CTC - Timerul se reseteaza cand atinge OCR1A

  TCCR1B |= (1 << CS12) | (1 << CS10);  // Seteaza prescaler la 1024: CS12=1, CS11=0, CS10=1

  TIMSK1 |= (1 << OCIE1A);        // Activeaza intreruperea la comparare 

  interrupts();
}


// Initializare UART manual (registri)
void uartInit() {
  uint16_t ubrr = 103;                   // Formula: UBRR = (F_CPU / (16 * BAUD)) - 1 => (16000000 / (16*9600)) - 1 = 103
  UBRR0H = (ubrr >> 8);                       // Scrie octetul superior al UBRR in registrul UBRR0H
  UBRR0L = ubrr;                              // Scrie octetul inferior al UBRR in registrul UBRR0L

  UCSR0B = (1 << TXEN0);                      // Activeaza TX 

  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);     // 8 biti de date, 1 stop bit, fara paritate
}


// Trimitere caracter prin UART
void uartSendChar(char c) {
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

// Trimitere string prin UART
void uartSendStr(const char* str) {
  while (*str) uartSendChar(*str++);
}

// Trimitere int ca text prin UART
void uartSendInt(int num) {
  char buf[10];
  itoa(num, buf, 10);
  uartSendStr(buf);
}

// Calculeaza ora curenta pornind de la ora de start
void getCurrentTime(int& hour, int& minute) {
  unsigned long elapsedMillis = millis() - startMillis;
  unsigned long totalMinutes = startHour * 60 + startMinute + elapsedMillis / 60000;
  hour = (totalMinutes / 60) % 24;
  minute = totalMinutes % 60;
}

// Activeaza mod noapte daca ora e intre 20:00 - 08:00
void updateNightMode(int hour) {
  nightMode = (hour >= 21 || hour < 8);
}

// Setup initial: senzori, LCD, timer, UART
void setup() {
  dht.begin();
  lcd.init();
  lcd.backlight();
  setupTimer1();
  uartInit();
  pinMode(BUZZER_PIN, OUTPUT);
  startMillis = millis();
  uartSendStr("Time,Temperature,Humidity\n");
}

// Loop principal
void loop() {
  currentTime = millis();

  // Actualizeaza ora curenta si mod
  int hour, minute;
  getCurrentTime(hour, minute);
  updateNightMode(hour);

  // Citeste date de la senzor
  temp = dht.readTemperature();
  hum = dht.readHumidity();

  // Afisaj LCD: alerta sau mod/temp/umiditate
  lcd.clear();
  if (temp > 25.0 && hum > 60.0 && !nightMode) {
    lcd.setCursor(0, 0);
    lcd.print("ALERTA: TEMP&HUM!");
  } else if (displayState == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Mode:");
    lcd.setCursor(0, 1);
    lcd.print(nightMode ? "NIGHT" : "DAY");
  } else if (displayState == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Temperatura:");
    lcd.setCursor(0, 1);
    lcd.print(temp);
    lcd.print(" C");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Umiditate:");
    lcd.setCursor(0, 1);
    lcd.print(hum);
    lcd.print(" %");
  }

  delay(200);

  // Transmitere UART o data la 10 secunde
  if (currentTime - lastCSVTime >= 10000) {
    if (hour < 10) uartSendChar('0');
    uartSendInt(hour);
    uartSendChar(':');
    if (minute < 10) uartSendChar('0');
    uartSendInt(minute);
    uartSendChar(',');
    uartSendInt((int)temp);
    uartSendChar('.');
    uartSendInt((int)((temp - (int)temp) * 10));
    uartSendChar(',');
    uartSendInt((int)hum);
    uartSendChar('.');
    uartSendInt((int)((hum - (int)hum) * 10));
    uartSendChar('\n');
    lastCSVTime = currentTime;
  }

  // Redare alarma cu buzzer in caz de alerta
  static bool alertActive = false;
  static unsigned long noteStart = 0;
  static int melodyIdx = 0;

  if (!nightMode && temp > 25.0 && hum > 60.0) {
    if (!alertActive) {
      alertActive = true;
      melodyIdx = 0;
    }

    if (currentTime - noteStart > currentMelody[melodyIdx].duration + 50) { 
      int freq = currentMelody[melodyIdx].frequency;
      int dur = currentMelody[melodyIdx].duration;

      if (freq > 0) {
        unsigned long halfPeriod = 1000000UL / freq / 2;
        unsigned long cycles = (dur * 1000UL) / (2 * halfPeriod);

        for (unsigned long i = 0; i < cycles; i++) {
          digitalWrite(BUZZER_PIN, HIGH);
          delayMicroseconds(halfPeriod);
          digitalWrite(BUZZER_PIN, LOW);
          delayMicroseconds(halfPeriod);
        }
      } else {
        delay(dur);
      }

      melodyIdx++;
      if (melodyIdx >= currentMelodyLength) melodyIdx = 0;
      noteStart = millis();
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    alertActive = false;
  }
}