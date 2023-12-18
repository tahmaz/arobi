/*
  firmware_robov3_right_foot
 */
#include <Servo.h>
#include <Wire.h>
#include <FastLED.h>

#define enapin 3
#define enbpin 11

#define in1pin 16
#define in2pin 17
#define in3pin 12
#define in4pin 13

#define sfrpin 2  //sensor front right
#define sflpin 4  //sensor front left
#define sbrpin 7 //sensor back right
#define sblpin 8 //sensor back left

#define batvpin A0 //battery voltage pin
#define batcpin A1 //battery current pin

#define echopin 5        // Pin received echo pulse, HC-SR04 ultra sound sensor 
#define triggerpin 6     // Pin sends shot pulse, HC-SR04 ultra sound sensor 

#define LED_PIN 10
#define NUM_LEDS    3
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

Servo servoa;
byte posa,posanew,posamin,posamax;

byte ledr = 0, ledg = 0, ledb = 0;
byte ledr_new = 0, ledg_new = 0, ledb_new = 0;

byte motrdr,motrdu;
byte motldr,motldu;
byte motrs = 0;
byte motls = 0;

byte delay_motr = 0;
byte delay_motl = 0;
unsigned long motr_last = 0;
unsigned long motl_last = 0;

unsigned long led_last = 0;
byte delay_led = 3; //sec

byte last_sensor = 0;

//HC-SR04 params
int distance; // the distance in cms         
unsigned long previousMicros = 0; // will sotre last time TRIGGER was updated    
long OnTime = 10; //microseconds of on-time     
long OffTime = 2; //microseconds of off-time    
int triggerState = LOW; // triggerState used to set it up   
long duration;

//lps calculation, loop per second
unsigned long last_time, now_time;
float lps;  
float lps_print_sec;

void setup()
{
  // Start up serial connection for monitor process
  Serial.begin(115200); // baud rate
  Serial.flush();

  // Start up i2c connection
  Wire.begin(0x8);
  Wire.onReceive(receiveEvent);

  servoa.attach(9,750,2800);
  //servob.attach(10,750,2800);

  posanew=posa=90;
  posamin=35;
  posamax=135;

  motrs=0;
  motls=0;
  motrdr=0;
  motldr=0;
  motrdu=0;
  motldu=0;
  
  servoa.write(posa);  

  pinMode(enapin, OUTPUT);
  pinMode(enbpin, OUTPUT);
  
  pinMode(in1pin, OUTPUT);
  pinMode(in2pin, OUTPUT);
  pinMode(in3pin, OUTPUT);
  pinMode(in4pin, OUTPUT);

  pinMode(sfrpin, INPUT);
  pinMode(sflpin, INPUT);
  pinMode(sbrpin, INPUT);
  pinMode(sblpin, INPUT);

  pinMode(echopin, INPUT);   
  pinMode(triggerpin, OUTPUT);  

  last_time = millis();
  lps_print_sec = 10;

  //led setup
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  for (int i = 0; i < NUM_LEDS; i++) {
     leds[i] = CRGB( 0, 0, 30 ); //little bit blue
  }

  FastLED.show();

  Serial.print("con8: setup completed."); 

}

void rforward()//right motor forward
{
    int enaoutput = map(motrs, 0, 9, 0 , 255); // Map the input value from 0 to 255
    analogWrite(enapin, enaoutput); // Send PWM signal to L298N Enable pin
    digitalWrite(in1pin, HIGH);
    digitalWrite(in2pin, LOW);
}

void rback()//right motor back
{
    int enaoutput = map(motrs, 0, 9, 0 , 255); // Map the input value from 0 to 255
    //Serial.println("rback: " + String(enaoutput));
    analogWrite(enapin, enaoutput); // Send PWM signal to L298N Enable pin
    digitalWrite(in1pin, LOW);
    digitalWrite(in2pin, HIGH);
}

void lforward()//left motor forward
{
    int enboutput = map(motls, 0, 9, 0 , 255); // Map the input value from 0 to 255
    //Serial.println("lforward: " + String(enboutput));
    analogWrite(enbpin, enboutput); // Send PWM signal to L298N Enable pin
    digitalWrite(in3pin, LOW);
    digitalWrite(in4pin, HIGH);
}

void lback()//left motor back
{
    int enboutput = map(motls, 0, 9, 0 , 255); // Map the input value from 0 to 255
    //Serial.println("lback: " + String(enboutput));
    analogWrite(enbpin, enboutput); // Send PWM signal to L298N Enable pin
    digitalWrite(in3pin, HIGH);
    digitalWrite(in4pin, LOW);
}

void rstop()//right motor stop
{
    analogWrite(enapin, 0); // Send PWM signal to L298N Enable pin
    digitalWrite(in1pin, LOW);
    digitalWrite(in2pin, LOW);
}

void lstop()//left motor stop
{
    analogWrite(enbpin, 0); // Send PWM signal to L298N Enable pin
    digitalWrite(in3pin, LOW);
    digitalWrite(in4pin, LOW);
}

void delayr_calc(int motrdu)
{   
  delay_motr = motrdu*100;
  motr_last = millis();
}

void delayl_calc(int motldu)
{   
  delay_motl = motldu*100;
  motl_last = millis();
}

//---------------Led functions----------------------------
void ChangeLed()
{
    if( ledr_new != ledr || ledg_new != ledg || ledb_new != ledb) {
        //set all 3 leds
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB( ledr_new, ledg_new, ledb_new);
        }

        FastLED.show();
        
        ledr = ledr_new;
        ledg = ledg_new;
        ledb = ledb_new;
    }
}

void loop()
{  

  now_time = millis();
  
  //write servos
  if(posa!=posanew){ 
    Serial.print("con8 command received:");
    //Serial.println(posanew);
    //Serial.println(posbnew);

    if(posa!=posanew){
      //if big than max then overrite
      if (posanew > posamax)
        posanew=posamax;
      //if small than min then overrite
      if (posanew < posamin)
        posanew=posamin;

      servoa.write(posanew);
      posa=posanew;
    } 
  }



  //right motor forward
  if(motrdr == 1){
    rforward();
    motrdr = -1;
    }

  //right motor back
  if(motrdr == 2){
    rback();
    motrdr = 0;
    }

  //left motor forward
  if(motldr == 1){
    lforward();
    motldr = -1;
    }

  //left motor back
  if(motldr == 2){
    lback();
    motldr = 0;
    } 
   
  if(now_time - motr_last > delay_motr){
    rstop();
  }

  if(now_time - motl_last > delay_motl){
    lstop();
  }

  //sensors_calc
  if(digitalRead(sfrpin) == 1)
    last_sensor += 1;
  if(digitalRead(sflpin) == 1)
    last_sensor += 1;
  if(digitalRead(sbrpin) == 1)
    last_sensor += 1;
  if(digitalRead(sblpin) == 1)
    last_sensor += 1;
  
  //read sensors and try to dont fall
  if(last_sensor == 1){
    if(digitalRead(sfrpin) == 1){
      motrdr=2; //back
      motrs=9;  //fast
      motrdu=1; // 100 ms
      delayr_calc(motrdu);
    }
    if(digitalRead(sflpin) == 1 ){
      motldr=2; //back
      motls=9;  //fast
      motldu=1; // 100 ms
      delayl_calc(motldu);
    }
    if(digitalRead(sbrpin) == 1 ){
      motrdr=1; //forward
      motrs=9;  //fast
      motrdu=1; // 100 ms
      delayr_calc(motrdu);
    }
    if(digitalRead(sblpin) == 1 ){
      motldr=1; //forward
      motls=9;  //fast
      motldu=1; // 100 ms
      delayl_calc(motldu);
    }
  }
  last_sensor = 0;
  //Serial.println("last_s: " + String(last_s));
/*
  if((triggerState == LOW) && (now_time - previousMicros >= OffTime))     
   {      
     triggerState = HIGH; // turn it on      
     previousMicros = now_time; // remember the time     
     digitalWrite(triggerpin, triggerState); // update the actual trigger     
   }    
   else if((triggerState == HIGH) && (now_time - previousMicros >= OnTime))    
   {           
     triggerState = LOW; // turn it off          
     previousMicros = now_time; // remember the time           
     digitalWrite(triggerpin, triggerState); // update the actual trigger      
   } 

 /*
  duration = pulseIn(echopin,HIGH);      
  //Serial.println(duration);
        
  distance = ((duration*0.034)/2);       
  //Serial.print("distance: "); 

  */ 
  if (now_time - last_time > lps_print_sec*1000) //print every lps_print_sec
  {
    
  int sensorValue = analogRead(batvpin);
  float voltage = sensorValue * (5.00 / 1023.00) * 2 - 0.3; //0.27 is adjust value
  //float voltage = map(sensorValue, 0, 1023, 0 , 5000) * 2;
  //Serial.println("Voltage: " + String(voltage) + " v");

  sensorValue = analogRead(batcpin);
  int current = (map(sensorValue, 0, 1023, 0 , 5000) - 1950)*100/185;
  //Serial.println("Current: " + String(current) + " ma" + " Sensor: " + String(sensorValue) + " Micros: " + String(currentMicros));

  //Serial.println("Voltage: " + String(voltage) + " v, Current: " + String(current) + " ma," + " Sensor: " + String(sensorValue) + ", LPS: " + String(lps/lps_print_sec) + " loop/sec");
  Serial.println(lps/lps_print_sec);
  Serial.println(voltage);
  last_time = now_time;
  lps = 0;
  }

  lps += 1;

//Led loop
  if(now_time - led_last > delay_led * 1000){
    ledr_new = 0;
    ledg_new = 0;
    ledb_new = 30;
  }
  ChangeLed();
 
}

void receiveEvent(int howMany)
{

  //commands 8.servoa.led_rgb.rdirection_speed_duration.ldirection_speed_duration.power-3digit.10*powersleep-1digit.100*powertimeout-1digit
  ////8.090.000.122.222
  String input = "";


  // Read any wire input
  while (Wire.available() > 0)
  {
    input += (char)  Wire.read(); // Read in one char at a time
    delay(5); // Delay for 5 ms so the next char has time to be received
  }
  Serial.print("con8: " + String(input.length()));
  Serial.print(input);
  if (input.length()==18 && input.substring(1, 2) == "8")
  {
    //get servo a position
    if(input.substring(3,4).toInt() != 4 ){
      posanew=input.substring(3,6).toInt();
      
      //servolarin mueyyen derece one ve ya arxaya hereketi, eger derece 2 ile bashlasa one, 3 ile bashlasa arxaya, mes 230 - 30 derece qabaga
      if(input.substring(3,4).toInt() == 2 )
        posanew=posa+input.substring(4,6).toInt();
      if(input.substring(3,4).toInt() == 3 )
        posanew=posa-input.substring(4,6).toInt();
    }

    //read led and change map 0-254
    ledr_new = map(input.substring(7,8).toInt(), 0, 9, 0 , 255);
    ledg_new = map(input.substring(8,9).toInt(), 0, 9, 0 , 255);
    ledb_new = map(input.substring(9,10).toInt(), 0, 9, 0 , 255);
    led_last = millis();
    
    //get motor a command  
    if(input.substring(11,12).toInt() != 4 ){ 
      motrdr=input.substring(11,12).toInt();
      motrs=input.substring(12,13).toInt();
      motrdu=input.substring(13,14).toInt();

      delayr_calc(motrdu);
    }
    
    //get motor b command 
    if(input.substring(15,16).toInt() != 4 ){
      motldr=input.substring(15,16).toInt();
      motls=input.substring(16,17).toInt();
      motldu=input.substring(17,18).toInt();

      delayl_calc(motldu);
    }

    
    
    
    //accept command
    Serial.println("con8: accepted");
    //Serial.println(input.substring(0));

  }
  else if (input != "")
  {
    Serial.print("con8: Command not accepted from controller1.");
  }

}