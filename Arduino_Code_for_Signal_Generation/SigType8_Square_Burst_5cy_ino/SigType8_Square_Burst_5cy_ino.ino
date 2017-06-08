//____________________________________________________________________
// LEARNING DIGITAL OSCILLOSCOPE USING ARDUINO AS SIGNAL GENERATOR
// 
// Date: 2013-12-01            Written by: Mukesh Soni 
//____________________________________________________________________
// OUTPUT DETAILS
//--------------------------------------------------------------------
// Pin # 11 : Square Wave Burst 16-17kHz Square Wave / 5 cycle burst
//____________________________________________________________________


int numCycle = 1;            // say 1 to 10 cycles
int interBurstDuration = 500;   // 250us min for 1-5 cycles, 500us min for >5 cycles     
void setup()
{
  // put your setup code here, to run once:
  pinMode(11, OUTPUT);  
  numCycle = 5;             //Duty Cycle in %
  interBurstDuration = 1000;  // 500us
}

void loop()
{
  // put your main code here, to run repeatedly:
  for (int i=1; i<= numCycle; i++)
  {
    digitalWrite(11,HIGH);
    delayMicroseconds(25);
    digitalWrite(11,LOW);
    delayMicroseconds(25);
  }  
  delayMicroseconds(interBurstDuration);
}



