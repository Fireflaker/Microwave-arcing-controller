/*
   notes:
   4*5 key data
   F700FF F7807F F740BF F7C03F
   F720DF F7A05F F7609F F7E01F
   F710EF F7906F F750AF F7D02F
   F730CF F7B04F F7708F F7F00F
   F708F7 F78877 F748B7 F7C837

     for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
    delay(150);
    // scroll one position left:
    lcd.scrollDisplayLeft();
  }

          lcd.setCursor(thisCol, thisRow);



  Used pins: 234567 LCD, 9 Servo, 11 IR, A0 and A1 for 2 temp probes, 13 for beeper, still have 4 alg pins and pin 8,10,12
*/

#include <digitalWriteFast.h>
#define highA 8
#define highB 10
#define lowA 12
#define lowB A3
#include <ResponsiveAnalogRead.h>
#include <IRremote.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
IRrecv irrecv(11);
decode_results results;
#include <Servo.h>
Servo myservo;
int arcPosition = 150;
int homePosition = 100;
int goval = 10;
unsigned long key_value = 0;

const int TEMP1 = A7;
const int TEMP2 = A1;
ResponsiveAnalogRead analog(A0, true);

int beepmillis = 0; // this is for non intterupt, not timing. for beep length go find beep function
int raw = 0;
int val = 0;
int temp = 888;
int tmax = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.print("Setup...");

  Serial.begin(115200);
  Serial.print("Setup...");

  myservo.attach(9);
  myservo.write(arcPosition);


  irrecv.enableIRIn(); // Start the receiver
  irrecv.blink13(true);

  pinModeFast(highA, OUTPUT);
  pinModeFast(highB, OUTPUT);
  pinModeFast(lowA, INPUT);
  pinModeFast(lowB, INPUT);

  pinModeFast(13, OUTPUT);// beeper


  unsigned long time_now = 0;

  analogReference(INTERNAL);
  pinMode(TEMP1, INPUT);

  lcd.setCursor(0, 1);
  lcd.print("Done & Passed");
  Serial.print("Setup is Done..");
}

void loop() {

  if (irrecv.decode(&results)) {

    if (results.value == 0XFFFFFFFF)
      results.value = key_value;
    lcd.setCursor(0, 0);
    lcd.clear();

    switch (results.value) {
      case 0xF700FF:
        lcd.print("A");
        break;
      case 0xF7807F:
        lcd.print("B");
        break;
      case 0xF740BF:
        lcd.print("On");
        cancel();
        cook();
        minute();
        start();
        beep();
        break;
      case 0xF7C03F:
        lcd.print("Off");
        break;

      case 0xF720DF:
        lcd.print("R");
        break ;
      case 0xF7A05F:
        lcd.print("G");
        break ;
      case 0xF7609F:
        lcd.print("B");
        break ;
      case 0xF7E01F:
        lcd.print("W");
        break ;

      case 0xF710EF:
        lcd.print("1");
        break ;
      case 0xF7906F:
        lcd.print("2");
        break ;
      case 0xF750AF:
        lcd.print("3");
        break ;
      case 0xF7D02F:
        lcd.print("4");
        break ;

      case 0xF730CF:
        lcd.print("Hm-");
        arcPosition--;
        break ;
      case 0xF7B04F:
        lcd.print("Hm+");
        arcPosition++;

        break ;
      case 0xF7708F:
        lcd.print("To-");
        homePosition--;
        break ;
      case 0xF7F00F:
        lcd.print("To+");
        homePosition++;

        break ;

      case 0xF708F7:
        lcd.print("Arc");
        Serial.print("Arc");
        arc();
        break ;
      case 0xF78877:
        lcd.print("dth");
        myservo.detach();
        break ;
      case 0xF748B7:
        lcd.print("Lft");
        myservo.attach(9);
        goval--;
        myservo.write(goval);
        delay(5);

        break ;
      case 0xF7C837:
        lcd.print("Rgt");
        myservo.attach(9);
        goval++;
        myservo.write(goval);
        delay(5);

        break ;
    }
    key_value = results.value;
    irrecv.resume();
  }



  lcd.setCursor(11, 1);
  lcd.print("?=");
  lcd.print(goval);
  lcd.setCursor(6, 1);
  lcd.print("To");
  lcd.print(homePosition);
  lcd.setCursor(0, 1);
  lcd.print("Hm");
  lcd.print(arcPosition);
  //*************************************Temperature
  //
  //    analog.update();
  //    Serial.print("raw:");
  //    Serial.print(raw = analog.getRawValue());
  //    Serial.print("val");
  //    Serial.println(val = analog.getValue());


  //**********************************************temp error ctrl
  //    if (val > 1010 || val < 10) {
  //      lcd.setCursor(5, 0);
  //      lcd.print("TempError");
  //      Serial.print("TempError");
  //    }
  //    else
  //    {
  //      temp = map(val, 917, 623, 20, 35);
  //
  //      Serial.print("Temp=");
  //      Serial.println(temp);
  //      //  temp = (temp * 9.0)/ 5.0 + 32.0;      convert to F
  //    }
  //    if (temp > 100)
  //    {
  //      lcd.setCursor(5, 0);
  //      lcd.print("TooHot");
  //    }


  //***************************************beeper

  if (millis() > beepmillis + 300) {
    digitalWriteFast(13, LOW);
  }
  else {
    digitalWriteFast(13, HIGH);
  }


}
// ======================================================end loop=================================

void arc()
{
  myservo.attach(9);
  myservo.write(arcPosition);
  delay(25);
  myservo.write(homePosition);
  delay(200);
  myservo.write(arcPosition);
  delay(25);
}

//******************************key munipulization, do not add delay use for loop only
void cancel()
{
  digitalWriteFast(highA, LOW);
  for (int i = 0; i <= 100; i++) { // read 100 times
    if (digitalReadFast(lowB)) {
      digitalWriteFast(highA, HIGH);
    }
  }
}

void cook()
{
  digitalWriteFast(highA, LOW);
  for (int i = 0; i <= 100; i++) { // read 100 times
    if (digitalReadFast(lowB)) {
      digitalWriteFast(highA, HIGH);
    }
  }
}

void start()
{
  digitalWriteFast(highA, LOW);
  for (int i = 0; i <= 100; i++) { // read 100 times
    if (digitalReadFast(lowB)) {
      digitalWriteFast(highA, HIGH);
    }
  }
}

void minute()
{
  digitalWriteFast(highA, LOW);
  for (int i = 0; i <= 100; i++) { // read 100 times
    if (digitalReadFast(lowB)) {
      digitalWriteFast(highA, HIGH);
    }
  }
}

void releasekey()
{
  pinModeFast(highA, INPUT);
  pinModeFast(highB, INPUT);
  pinModeFast(lowA, INPUT);
  pinModeFast(lowB, INPUT);
}

void holdkey()
{
  pinModeFast(highA, OUTPUT);
  pinModeFast(highB, OUTPUT);
  pinModeFast(lowA, INPUT);
  pinModeFast(lowB, INPUT);
}

void beep()
{
  beepmillis = millis();
}

