//____________________________________________________________________
// LEARNING DIGITAL OSCILLOSCOPE USING ARDUINO AS SIGNAL GENERATOR
// 
// Date: 2013-12-01            Written by: Mukesh Soni 
//____________________________________________________________________
// OUTPUT DETAILS
//--------------------------------------------------------------------
// Pin # 7 : PWM - 1 kHz, 20 % Duty Cycle
// Pin # 8 : PWM - 1 kHz, 50 % Duty Cycle
// Pin # 9 : PWM - 1 kHz, 80 % Duty Cycle
//____________________________________________________________________

void setup()
{
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode (9, OUTPUT);  
}

void loop()
{
  // put your main code here, to run repeatedly:
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  delayMicroseconds(195);
  digitalWrite(7, LOW);
  delayMicroseconds(295);
  digitalWrite(8, LOW);
  delayMicroseconds(295);
  digitalWrite(9, LOW);
  delayMicroseconds(195);  
}


