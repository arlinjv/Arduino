// put out dummy address and temp values to test processing plot program




#include "WProgram.h"
void setup();
void loop();
void setup()
{
  Serial.begin(9600);
}


void loop()
{
  Serial.println("address 4023094832048567 F= 74.50");
  delay(1000);
  Serial.println("address 4023094832048522 F= 73.40");
  delay(1000);
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

