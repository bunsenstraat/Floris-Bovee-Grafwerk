#include <IRremote.h>
#include <SimpleTimer.h>
#include "RTClib.h"



RTC_DS3231 rtc;


SimpleTimer timer;
SimpleTimer timer2;

// IR MODULE
int RECV_PIN = 9;
int LED_PIN = 10;

// The duration of a dash is three times the duration of a dot. Each dot or dash within a character is followed by period of signal absence, called a space, equal to the dot duration. The letters of a word are separated by a space of duration equal to three dots, and the words are separated by a space equal to seven dots.

int SHORT_DL = 100;
int LONG_DL = SHORT_DL*3;

int CH_DL = SHORT_DL*3;
int SPACE_DL = SHORT_DL;
int WORD_DL = SHORT_DL*7;

char tekst[] = "-.- .-. .- -.- . -. / --. .- .- - / -.. --- --- .-. -.-.-- / .. -.. . .- .-.. . -. / --- -. - .-. ..- .. -- / .--- . / -. .. . - -.-.--";
int position = 0;
int timetowait = 0;
int status = 0; // 0 = stop, 1 = started, 2 = 

IRrecv irrecv(RECV_PIN);
decode_results results;
//




void setup() {
    Serial.begin(57600);
    irrecv.enableIRIn(); // Start the IR receiver
    #ifndef ESP8266
    //  while (!Serial); // wait for serial port to connect. Needed for native USB
    #endif
    if (! rtc.begin()) {
    //  Serial.println("Couldn't find RTC");
    }
    if (rtc.lostPower()) {
    //  Serial.println("RTC lost power, let's set the time!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));



    analogWrite(LED_PIN,0);
    timer.setTimeout(1000, readCharacter);
    timer.setInterval(1000, readTime);
}

void  dumpInfo (decode_results *results)
{
  // Check if the buffer overflowed
  if (results->overflow) {
    Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWBUF");
    return;
  }
  ircode(results);
}

void  ircode (decode_results *results)
{

  switch(results->value){

    case 10779859:
      Serial.print("stop");
      status = 0;
    break;
    case 3358177814:
      Serial.print("start");
      if(status == 0){
        position = 0;
        status = 1;
        timer.setTimeout(timetowait, readCharacter);
      }
    break;
    case 3228363185:
     Serial.print("speed up");

      break;
    case 2607444189:
     Serial.print("speed down");

      break;
  }
}


void readCharacter(){
  
  if(status == 1){
  //
   if(tekst[position] == '-'){
    Serial.println(tekst[position]);
    analogWrite(LED_PIN,255);
    timetowait = SPACE_DL;
    timer.setTimeout(LONG_DL, lightsOff);
   }
   if(tekst[position] == '.'){
    Serial.println(tekst[position]);
    analogWrite(LED_PIN,255);
    timetowait = SPACE_DL;
    timer.setTimeout(SHORT_DL, lightsOff);
   }
   if(tekst[position] == ' '){
    Serial.println(tekst[position]);
    analogWrite(LED_PIN,0);
    timetowait = 0;
    timer.setTimeout(CH_DL, lightsOff);
   }
   if(tekst[position] == '/'){
    Serial.println(tekst[position]);
    timetowait = 0;
    analogWrite(LED_PIN,0);
    timer.setTimeout(WORD_DL, lightsOff);
   }
  }
}

void lightsOff(){
  analogWrite(LED_PIN,0);

  
  position++;
  if(position == sizeof(tekst)-1){
          position = 0;
          Serial.println("stopping");
          status = 0;
  }
  timer.setTimeout(timetowait, readCharacter);
}

void readTime(){
  if(status == 0){
    DateTime now = rtc.now();
    Serial.print(now.minute(), DEC);
    if(now.minute() % 5 == 0){
       status = 1;
       timer.setTimeout(timetowait, readCharacter);
    }
  }
}

void loop() {

  if (digitalRead(RECV_PIN)) {

  }else{
    if (irrecv.decode(&results)) {
        dumpInfo(&results);
        irrecv.resume(); // Receive the next value
      }
    
  }

  
  timer.run();





/// DateTime now = rtc.now();
//  
//  Serial.print(now.year(), DEC);
//  Serial.print('/');
//  Serial.print(now.month(), DEC);
//  Serial.print('/');
//  Serial.print(now.day(), DEC);
//
//  Serial.print(now.hour(), DEC);
//  Serial.print(':');
//  Serial.print(now.minute(), DEC);
//  Serial.print(':');
//  Serial.print(now.second(), DEC);
//  Serial.println();
}
