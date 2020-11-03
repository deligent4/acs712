
const byte  inputPin = A0;             // sensor input
const float sensitivity = 0.185;    // for 5Amps version,for 20 and 30 Amps check datasheet

int calButton = 3;
int buttonState;
float sensorOut = 0.0;
float calvalue = 0.0;

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2);

void setup() {
  
  lcd.init();          // initialize the lcd 
  lcd.backlight();     //always turn on the backlight of LDC module
  Serial.begin(9600);  
  pinMode (calButton, INPUT_PULLUP);
}

void loop() {

  // read sensor current and display on serial monitor
  measureCurrent();

  buttonState = digitalRead (calButton);
  if (buttonState == LOW)  {
     calvalue = calibrationFactor();
     delay(100);

      buttonState = HIGH;
  }
  delay(100);
}


// measure current
int32_t measureCurrent() {

  // variables for taking average measurements
  int32_t sensorValue = 0;
  float Vcc = 0;

  // take the average of 100 samples
  for (int i = 0; i < 100; i++) {
    sensorValue =  sensorValue + analogRead(inputPin);
    Vcc = Vcc + readVcc();
  }
  sensorValue = (sensorValue / 100);
  Vcc = Vcc / 100;

  float sensorOut = ((Vcc / 1023) * sensorValue);
  int32_t currentmA = ((sensorOut - calvalue ) / sensitivity );
/*
  Serial.print("sensor Value ");
  Serial.println(sensorValue);
  Serial.print("Vcc ");
  Serial.println(Vcc);
 // Serial.print("calibration Factor ");
  //Serial.println(calibrationFactor());
  Serial.print("sensor out ");
  Serial.println(sensorOut);
  Serial.print("current value ");
  Serial.println(currentmA);
  
  Serial.println("  ");
 */ 
  //Serial.print("current value ");
  //Serial.println(calvalue);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("    Current:    ");
  lcd.setCursor(3,1);
  lcd.print(currentmA);
  lcd.setCursor(9,1);
  lcd.print("mA");

  return (currentmA);
}

float calibrationFactor() {

  int32_t sensorValue1 = 0;
  float Vcc1 = 0;

  // take the average of 100 samples
  for (int j = 0; j < 100; j++) {
    sensorValue1 =  sensorValue1 + analogRead(inputPin);
    Vcc1 = Vcc1 + readVcc();
  }
  sensorValue1 = (sensorValue1 / 100);
  Vcc1 = Vcc1 / 100;
  float sensorOut1 = ((Vcc1 / 1023) * sensorValue1);
  return (sensorOut1); 
}

// ---------------------------------------------------
// calculate Vcc in mV from the 1.1V reference voltage
// ---------------------------------------------------
int32_t readVcc() {
  int32_t result = 5000L;

  // read 1.1V reference against AVcc - hardware dependent
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
     ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
     #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
     ADMUX = _BV(MUX5) | _BV(MUX0);
     #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
     ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

#if defined(__AVR__)
  delay(2);                         // wait for Vref to settle
  ADCSRA |= _BV(ADSC);              // convert, result is stored in ADC at end
  while (bit_is_set(ADCSRA, ADSC)); // measuring
  result = ADCL;                    // must read ADCL (low byte) first - it then locks ADCH
  result |= ADCH << 8;              // unlocks both
  result = 1125300L / result;       // back-calculate AVcc in mV; 1125300 = 1.1*1023*1000
#endif

  return (result);
}
