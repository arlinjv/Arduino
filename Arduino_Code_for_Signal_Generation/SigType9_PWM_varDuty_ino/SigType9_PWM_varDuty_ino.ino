//____________________________________________________________________
// LEARNING DIGITAL OSCILLOSCOPE USING ARDUINO AS SIGNAL GENERATOR
// 
// Date: 2013-12-01            Written by: Mukesh Soni 
//____________________________________________________________________
// OUTPUT DETAILS
//--------------------------------------------------------------------
// Pin # 3  : PWM of frequency 31.37 kHz, Duty Cycle - 50%
// Pin # 11 : PWM of frequency 31.37 kHz, Duty Cycle - 20%
//
// CONFIGURATION: None
//____________________________________________________________________

//int dutyCycle = 10;            // 5 to 95%
    
void setup()
{
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);  
  TCCR2B = TCCR2B & 0b11111000 | 0x01;  
}

void loop()
{
  // put your main code here, to run repeatedly:
  analogWrite(3, 127);
  analogWrite(11, 51);
}



