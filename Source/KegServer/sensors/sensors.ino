#include <arduino.h>
#include <SoftwareSerial.h>
#include <OneWire.h> 

#define SOLENOID 4
#define FLOW 2 // Pin 2...
#define TEMP 5

OneWire ds(TEMP); // on digital pin 5

float volume;
volatile int flowCount = 0;
int flowPin = 0, analog;
unsigned long time, totalPulses;

void setup()
{
  Serial.begin(9600); 
  pinMode(SOLENOID, OUTPUT);
  digitalWrite(SOLENOID, LOW);
  pinMode(FLOW, INPUT);
  
  //attachInterrupt(FLOW, flowCounter, FALLING);
}

void loop()
{
  /*
  if ((millis() - time) >= 1000) {
    detachInterrupt(flowPin);
    totalPulses += flowCount;
    time = millis();
    volume = totalPulses/6100.0;      //liter
    Serial.print("flow = ");
    Serial.println(flowCount); 
    
    Serial.print("total = ");
    Serial.println(totalPulses); 

    Serial.print("volume = ");
    Serial.println(volume);
    flowCount = 0;
    attachInterrupt(flowPin, flowCounter, FALLING);
  }
  
  return;
  */
  float temperature = getTemp();
  Serial.print("Temperature: ");
  Serial.println(temperature);
  
  int val = digitalRead(FLOW);
  Serial.print("Flow: ");
  Serial.println(val);
  
  return;
  
  digitalWrite(SOLENOID, LOW);
  Serial.println("Closed");
  delay(2000);
  
  digitalWrite(SOLENOID, HIGH);
  Serial.println("Open");
  delay(5000);
  
  
}

void flowCounter() {
  flowCount++;
}

float getTemp(){
 //returns the temperature from one DS18S20 in DEG Celsius

 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   //no more sensors on chain, reset search
   ds.reset_search();
   return -1000;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }

 ds.reset();
 ds.select(addr);
 ds.write(0x44,1); // start conversion, with parasite power on at the end

 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); // Read Scratchpad

 
 for (int i = 0; i < 9; i++) { // we need 9 bytes
  data[i] = ds.read();
 }
 
 ds.reset_search();
 
 byte MSB = data[1];
 byte LSB = data[0];

 float tempRead = ((MSB << 8) | LSB); //using two's compliment
 float TemperatureSum = tempRead / 16;
 
 return TemperatureSum;
 
}

/*
int pin = 7;
unsigned long duration;

void setup()
{
  Serial.begin(9600);
  pinMode(pin, INPUT);
}

void loop()
{
  duration = pulseIn(pin, HIGH);
  Serial.print(duration);                                  // End of Line
  Serial.println("");
  //delay(200);
}
*/
