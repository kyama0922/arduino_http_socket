// test.h

#ifndef _TEST_h
#define _TEST_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h>
#include <Ethernet.h>

#include "Metro.h"

class cHTTP_SOCKET
{
private:
	EthernetClient client;
	Metro* timer;
	bool HTTPRequest(const char* url);
public:
	void Initialize(uint8_t* macaddr , IPAddress ip, uint32_t time_out = 3000);

	uint32_t ReadCommand(const char* url, char* out_buffer , unsigned int len);
	
	EthernetClient* GetClient();
};

#endif

