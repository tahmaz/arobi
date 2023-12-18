/*
  firmware_robov3_right_foot
 */
#include <Servo.h>
#include <Wire.h>
Servo servoa,servob;

int power,powernew,powermin,powermax;
int powersleep,powersleepnew,powersleepmin,powersleepmax;
int powertimeout,powertimeoutnew,powertimeoutmin,powertimeoutmax;

int posa,posanew,posamin,posamax;
int posb,posbnew,posbmin,posbmax;

int motrd,motrs;
int motld,motls;

int led = 13; // Pin 13
boolean ledstate=0;
int Powerpin = 3;

const int in1 = 14;
const int in2 = 15;
const int in3 = 16;
const int in4 = 17;

int delay_motr = 0;
int delay_motl = 0;

void setup()
{
  //pinMode(led, OUTPUT); // Set pin 13 as digital out
  pinMode(Powerpin, OUTPUT);

  // Start up serial connection for monitor process
  Serial.begin(115200); // baud rate
  Serial.flush();

  // Start up i2c connection
  Wire.begin(0x8);
  Wire.onReceive(receiveEvent);

  servoa.attach(9,750,2800);
  servob.attach(10,750,2800);
  
  powernew=240;
  power=240;
  powermin=0;
  powermax=255;
  powersleepnew=0;
  powersleep=0;
  powersleepmin=0;
  powersleepmax=50;
  powertimeoutnew=500;
  powertimeout=2000;
  powertimeoutmin=100;
  powertimeoutmax=4500;

  posanew=posa=90;
  posamin=35;
  posamax=135;
  posbnew=posb=0;
  posbmin=35;
  posbmax=135;

  motrs=0;
  motls=0;
  motrd=0;
  motld=0;
  
  servoa.write(posa);  
  servob.write(posb);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  Serial.print("con8: setup completed."); 

  //commands 8.servoa.servob.rdirection_duration.ldirection_duration.power-3digit.10*powersleep-1digit.100*powertimeout-1digit
  ////8.090.000.12.22.255.0.8

}

void loop()
{  
  digitalWrite(led, ledstate);
  if(ledstate==0)
    ledstate=1;
  else
    ledstate=0;

  //write servos
  if(posa!=posanew || posb!=posbnew ){ 

    if (powernew > powermax)
      powernew=powermax;
    if (powernew < powermin)
      powernew=powermin;
    //poweron
    analogWrite(Powerpin,powernew);
    power=powernew;
    Serial.print("con8 command received:");
    //Serial.println(posanew);
    //Serial.println(posbnew);
    //Serial.println(powernew);
    //Serial.println(powersleepnew);
    //Serial.println(powertimeoutnew);

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
    if(posb!=posbnew){
      //if big than max then overrite
      if (posbnew > posbmax)
        posbnew=posbmax;
      //if small than min then overrite
      if (posbnew < posbmin)
        posbnew=posbmin;

      servob.write(posbnew);
      posb=posbnew;
    }

    if (powertimeoutnew > powertimeoutmax)
      powertimeoutnew=powertimeoutmax;
    if (powertimeoutnew < powertimeoutmin)
      powertimeoutnew=powertimeoutmin;
    //power off
    //delay(powertimeoutnew);
    powertimeout=powertimeoutnew;
  }


  if (powertimeout==0){
    if (powersleepnew > powersleepmax)
      powersleepnew=powersleepmax;
    if (powersleepnew < powersleepmin)
      powersleepnew=powersleepmin;
    analogWrite(Powerpin,powersleepnew);
    powersleep=powersleepnew;
  }

  if(powertimeout > 0)
    powertimeout=powertimeout-1;
    
  //if (powertimeout!=0)
  //Serial.println(powertimeout);


  //right motor forward
  if(motrd == 1){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    motrd = -1;
    }

  //right motor back
  if(motrd == 2){
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    motrd = 0;
    }

  //left motor forward
  if(motld == 1){
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    motld = -1;
    }

  //left motor back
  if(motld == 2){
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    motld = 0;
    } 
   
  delay(1);
  if(delay_motr < 1){
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }

  if(delay_motl < 1){
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }

  delay_motr = delay_motr - 1;
  delay_motl = delay_motl - 1;
}

void receiveEvent(int howMany)
{
  String input = "";


  // Read any wire input
  while (Wire.available() > 0)
  {
    input += (char)  Wire.read(); // Read in one char at a time
    delay(5); // Delay for 5 ms so the next char has time to be received
  }
  Serial.print("con8: " + String(input.length()));
  Serial.print(input);
  if (input.length()==24 && input.substring(1, 2) == "8")
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
    
    //get servo b position
    if(input.substring(7,8).toInt() != 4 ){
      posbnew=input.substring(7,10).toInt();

      //servolarin mueyyen derece one ve ya arxaya hereketi, eger derece 2 ile bashlasa one, 3 ile bashlasa arxaya, mes 230 - 30 derece qabaga
      if(input.substring(7,8).toInt() == 2 )
        posbnew=posb+input.substring(8,10).toInt();
      if(input.substring(7,8).toInt() == 3 )
        posbnew=posb-input.substring(8,10).toInt();
    }
    
    motrd=input.substring(11,12).toInt();
    motrs=input.substring(12,13).toInt();

    motld=input.substring(14,15).toInt();
    motls=input.substring(15,16).toInt();

    delay_motr = motrs*3*20;
    delay_motl = motls*3*20;

    powernew=input.substring(17,20).toInt();
    powersleepnew=10*input.substring(21,22).toInt();
    powertimeoutnew=500*input.substring(23,24).toInt();
    
    //accept command
    Serial.println("con8: accepted");
    //Serial.println(input.substring(0));

  }
  else if (input != "")
  {
    Serial.print("con8: Command not accepted from controller1.");
  }

}