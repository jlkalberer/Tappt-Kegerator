#include "Wifly.h"
#include <SoftwareSerial.h>
WiflyClass Wifly = WiflyClass(2,3);

void setup()
{
	Serial.begin(9600);
	Serial.write("Test");
	Wifly.init();
	Wifly.setConfig("casanova","bitchhunter");
	Wifly.join("casanova");
	Wifly.checkAssociated();
	while(!Wifly.connect("192.168.1.138","90"));
	Wifly.writeToSocket("Connected!");
}
void loop()
{
	if(Wifly.canReadFromSocket())
	{
		Wifly.readFromSocket();
	}
	if(Serial.available())
	{
		Wifly.print((char)Serial.read());
	}
}
