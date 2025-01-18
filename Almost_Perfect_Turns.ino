/******************************************************************************
Arduino-Based Custom Motor Control Designed by Me
******************************************************************************/

/////SECTION FOR LIBRARIES, CLASSES, CONSTANTS AND VARIABLES/////
//////MOTORS
// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
#define PWMA 3 // pin number that will set the speed to motor A
#define AIN1 4 // pin number that will pilot the motor A
#define AIN2 5 // pin number that will pilot the motor A
#define STBY 6 // pin number that can put in standby the motor driver
#define BIN1 7 // pin number that will pilot the motor B
#define BIN2 8 // pin number that will pilot the motor B
#define PWMB 9 //pin number that will set the speed to motor B
#define leftOC 10 // pin number that will read from the left optocoupler
#define rightOC 11 // pin number that will read from the right optocoupler
// Constants that are used to choose what the robot will do
#define forward 1
#define backward -1
#define left -1
#define right 1
#define clockwise 1
#define counterclockwise -1
// Constant about the wheel and encoder specifications
#define pulsesPerRotation 20
#define cmPerRotation 21
// variables
int speed = 170; //from 0 to 255
int delayingBraking = 0; //if there is the need to delay the braking instruction, put a time here or add it to the code
int ms_delay = 0;
int i;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Setting all the pins connected to the motor as outputs
  for (i = 3; i < 10; i++){
    pinMode(i, OUTPUT);
  }
  digitalWrite(STBY, HIGH);
  analogWrite(PWMA, speed);
  analogWrite(PWMB, speed);
  //Setting all the pins connected to the optocouplers as inputs
  pinMode(11, INPUT);
  pinMode(10, INPUT);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  go(forward, 20/*centimeters*/);
  delay(ms_delay);
  brake();
  go(backward, 20/*centimeters*/);
  delay(ms_delay);
  brake();

  turn(left, forward, 90/*degrees*/);
  delay(ms_delay);
  brake();
  turn(left, backward, 90/*degrees*/);
  delay(ms_delay);
  brake();
  turn(right, forward, 90/*degrees*/);
  delay(ms_delay);
  brake();
  turn(right, backward, 90/*degrees*/);
  delay(ms_delay);
  brake();

  spin(clockwise, 360/*degrees*/);
  delay(ms_delay);
  brake();
  spin(counterclockwise, 360/*degrees*/);
  delay(ms_delay);
  brake();

}

void brake() {
  delay(delayingBraking);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  delay(1000);
}

void go(int direction, int travelLength) 
{
  int pulsesCalculated = (travelLength*pulsesPerRotation)/cmPerRotation;
  if (direction == forward) 
  {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } 
  else if (direction == backward) 
  {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  }
  if (pulsesCalculated != 0)
  {
    Optocoupler(pulsesCalculated, 1011);
  }
}

void turn(int directionX, int directionY, int degrees) 
{

  int pulsesCalculated = degrees*(80/1)/360; // Usa costanti
  //that 80 is because: with one wheel only to make a 360 degrees turn it needs that wheel needs to make 4 times a complete rotation
  //a complete rotation corresponds to 20 pulses (in my case, because my encoder disk has 20 holes); 4*20=80

  if(directionX == left)
  {
    if(directionY == forward)
    {
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
    }
    if(directionY == backward)
    {
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, HIGH);
    }
    if (pulsesCalculated != 0)
    {
      Optocoupler(pulsesCalculated, leftOC);
    }
  }
  if(directionX == right)
  {
    if(directionY == forward)
    {
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, LOW);
    }
    if(directionY == backward)
    {
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, HIGH);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, LOW);
    }
    if (pulsesCalculated != 0)
    {
      Optocoupler(pulsesCalculated, rightOC);
    }
  }

}

void spin(int direction, int degrees) 
{

  //that 80 is because: with one wheel only to make a 360 degrees turn it needs that wheel needs to make 4 times a complete rotation
  //a complete rotation corresponds to 20 pulses (in my case, because my encoder disk has 20 holes); 4*20=80
  //with two wheels, each one with a different rotating direction, needs half of the movements in order to make the robot do a 360 spin
  //each wheel will do 4*20=80 --> 80/2=40 pulses per each wheel
  int pulsesCalculated = degrees*(80/2)/360; // Usa costanti

  if (direction == clockwise) 
  {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  } 
  else if (direction == counterclockwise) 
  {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  }
  if (pulsesCalculated != 0)
  {
    Optocoupler(pulsesCalculated, 1011);
    
  }

}

void Optocoupler(int pulsesMax, int pinOC) 
{
  int pulses = 0;
  int L_pulses = 0;
  int R_pulses = 0;
  int presetOC = 100;
  int L_presetOC = 100;
  int R_presetOC = 100;
  int outputOC = 0;
  int L_outputOC = 0;
  int R_outputOC = 0;
  int TOTpulses = 0;

  if((pinOC == 10) || (pinOC == 11))
  {
    while (pulses <= pulsesMax)
    {
      outputOC = digitalRead(pinOC);
      if (outputOC != presetOC) 
      {
        if (outputOC == HIGH) 
        {
          pulses = pulses+1;
        }
      }
      presetOC = outputOC;
      Serial.print("Pulses: ");
      Serial.println(pulses);
    }
  }

  if(pinOC == 1011)
  {

    while ((L_pulses <= pulsesMax) && ((R_pulses <= pulsesMax))) 
    {

      if(R_pulses <= pulsesMax)
      {
        R_outputOC = digitalRead(11);
        if (R_outputOC != R_presetOC) 
        {
          if (R_outputOC == HIGH) 
          {
            R_pulses = R_pulses+1;
          }
        }
        R_presetOC = R_outputOC;
      }
      else
      {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, LOW);
      }

      if(L_pulses <= pulsesMax)
      {
        L_outputOC = digitalRead(10);
        if (L_outputOC != L_presetOC) 
        {
          if (L_outputOC == HIGH) 
          {
            L_pulses = L_pulses+1;
          }
        }
        L_presetOC = L_outputOC;
      }
      else
      {
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, LOW);
      }  

    }

  }
  brake();
}
