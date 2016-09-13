/*
Objective: Create a comprehensive temperature monitoring and logging application. Temperature device and LCD 
           interfaces will be encapsulated in objects.
           
           
useful information for creating array of OneWire objects: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1262559739
- of course intention here is to create array of OneWire device objects (example creates array of OneWire bus objects)

Using objects will require creating .h and .cpp files. Should be able to rename tabs with appropriate file types. Then probably need to add include in main tab. 
This begs the question of where do I put include for onewire library. 

Alternate approach: use array of addresses. Derive size of array with enumerate routine. (Will really be a 2d array since each address is a 8 byte arrary in itself.)


Question remains of what to do with LCD code (refer to onewire library examples http://www.arduino.cc/playground/Learning/OneWire

possible Loop pseudocode

reset ds search

enumerateTherms

for each numTherms
  get address
