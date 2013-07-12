#include <SPI.h>
#include <Ethernet.h>

#include "http_socket.h"

cHTTP_SOCKET HTTP_SOCKET;

// Ethernet Shild MACƒAƒhƒŒƒX
byte mac[] = { 0x90,0xA2,0xDA,0x0D,0xC9,0xE6 };

void setup() {
	Serial.begin(9600);
	HTTP_SOCKET.Initialize(mac , IPAddress(192,168,1,1) , 3000);
	delay(1000);
}

void loop(){

	char test[128] = {0};
	if(HTTP_SOCKET.ReadCommand( "http://localhost/" , test, sizeof(test)))
	{
		Serial.print(dest);
	}
}
