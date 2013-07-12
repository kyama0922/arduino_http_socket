// 
// 
// 
#include "http_socket.h"


void cHTTP_SOCKET::Initialize(uint8_t* macaddr , IPAddress ip , uint32_t time_out){
	Ethernet.begin(macaddr,  ip );
	delay(1000);
	timer = new Metro(0);
	timer->interval(time_out);
}

#define _DEBUG

bool cHTTP_SOCKET::HTTPRequest(const char* url){
	this->client.stop();
	this->client.flush();

	char index[3] = {0};
	if(memcmp(url , "http://" , 7) == 0)
	{
		index[0] = 7;
	}
	{
		int i;
		for(i=index[0]; i<strlen(url);i++)
		{
			if(url[i] == ':')
			{
				index[1] = i;
			}
			else if(url[i] == '/')
			{
				index[2] = i;
				break;
			}
		}
	}

	uint32_t size = (index[1]?index[1]:index[2]) - index[0];
	char* host_name = (char*)malloc(size+1);
	memset(host_name , 0 , size+1);
	memcpy(host_name , &url[index[0]] , size);
	//ポートの設定
	int port = 80;
	if(index[1]){
		size = index[2] - index[1] + 1;
		char* port_char = (char*)malloc(size);
		memset(port_char , 0 , size);
		memcpy(port_char , &url[ index[1]+1 ]  , size);
		port = atoi(port_char);
		free(port_char);
	}
#ifdef _DEBUG
	Serial.println(host_name);
	Serial.println(port);
	Serial.println("connecting...");
#endif
	this->client.setTimeout(1000);
	int ref = this->client.connect(host_name , port);
	free(host_name);
	if(ref == 0){
		Serial.println("connection failed\n");
		return false;
	}
	char* http_request = (char*)malloc(strlen(&url[index[2]]) + 14);
#ifdef _DEBUG
	sprintf( http_request , "GET %s HTTP/1.0" , &url[index[2]]);
#endif
	Serial.println(http_request);
	this->client.println(http_request);
	this->client.println("");
	free(http_request);

	return true;
}

uint32_t cHTTP_SOCKET::ReadCommand(const char* url, char* out_buffer , unsigned int len){
#define TMP_ARRAY 256
	uint32_t ref = 0;
	if (this->client.available()) {
		char work;
		uint32_t i;
		char tmp_tlb[TMP_ARRAY] = {0};
		int old_head;
		while((work = this->client.read()) != -1){
			tmp_tlb[i++] = work;
			if(i > (TMP_ARRAY-1))
			{
				i = 0;
			}
			//改行コードを探す
			if(work == 0x0A)
			{
				//HTTPプロトコルか確認
				if(memcmp(tmp_tlb , "HTTP/1.1" , 8) == 0)
				{
					if(memcmp(&tmp_tlb[9] , "200" , 3) != 0)
					{
						return 0;
					}
				}
				else if(old_head == 0x0A0D)
				{
					ref = strlen(tmp_tlb);
					//ref = ref<len? ref : len;
					memcpy( out_buffer , tmp_tlb , ref);
					break;
				}
				old_head = 0;
				memcpy( &old_head , tmp_tlb , sizeof(int));
				memset(tmp_tlb , 0 , (TMP_ARRAY-1));
				i = 0;
			}
		}
		if(!ref && old_head == 0x0A0D)
		{
			ref = strlen(tmp_tlb);
			//ref = ref<len? ref : len; 
			memcpy( out_buffer , tmp_tlb , ref);
		}
		//ソケットを閉じる
		this->client.stop();
		this->client.flush();

	}
	else if(timer->check() == 1){
		this->HTTPRequest(url);
		timer->reset();
	}

	return ref;
}
