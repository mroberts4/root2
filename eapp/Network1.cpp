#include "Network1.h"
#include <vector>
#include <iostream>
#include "string.h"

#include "eapp_utils.h"
#include "edge_call.h"
#include "syscall.h"

#include <chrono>
#include <iomanip>

#include "crypto.h"
#include "aes.hpp"

#define OCALL_PRINT_STRING_12 1
#define OCALL_PRINT_TIME 5
#define OCALL_PRINT_BUFFER 9
#define OCALL_WAIT_FOR_KEY 10

uint8_t pubkey[AES_KEYLEN] = {
			(uint8_t)0x1a, (uint8_t)0x2b, (uint8_t)0x3b, (uint8_t)0x4d,
			(uint8_t)0x5e, (uint8_t)0x6f, (uint8_t)0x71, (uint8_t)0x82,
			(uint8_t)0x93, (uint8_t)0x14, (uint8_t)0x25, (uint8_t)0x36,
			(uint8_t)0x47, (uint8_t)0x58, (uint8_t)0x69, (uint8_t)0x7a 
		};
		
uint8_t netiv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

extern "C" 
{
  unsigned long ocall_print_string(char* string) 
  {
      unsigned long retval;
      ocall(OCALL_PRINT_STRING_12, string, strlen(string) + 1, &retval, sizeof(unsigned long));
      return retval;
  }
  
  unsigned long ocall_print_time(char* string) 
  {
      unsigned long retval;
      ocall(OCALL_PRINT_TIME, string, strlen(string) + 1, &retval, sizeof(unsigned long));
      return retval;
  }
  
  unsigned long ocall_print_buffer(char* string) 
	{
		unsigned long retval;
		ocall(OCALL_PRINT_BUFFER, string, strlen(string) + 1, &retval, sizeof(unsigned long));
		return retval;
	}

	void concatStrings(char* dest, char* concat) 
	{
    size_t destLen = strlen(dest);
    size_t concatLen = strlen(concat);

    // Ensure there is enough space in dest for concat and null terminator
    if (destLen + concatLen + 1 >= 2048) 
    {
      return;  // Not enough space, return without concatenating
    }

    // Move to the end of dest string
    dest += destLen;

    // Copy the concat string to dest
    while (*concat) 
    {
      *dest = *concat;
      dest++;
      concat++;
    }

    // Add null terminator
    *dest = '\0';
	}
}

using namespace std;

Network::Network()
{
	ocall_print_time("Network Init 1 Start");
	
	ocall_print_buffer("Initializing Network 1...\n");
	
	m_Readdata = new ReadData(1, 224, 224, 3);
	
	// First Layer
	m_Layers_bn = new Layers_Bn(3, 22, 224, 2, 1);  // Reduced from 32 to 22 (32 / sqrt(2))
	
	ocall_print_buffer("Initializing Network 1 Done...\n");
	
	ocall_print_time("Network Init 1 End");
	
	int outputSize = m_Layers_bn->GetOutputSize();
	char print[32];
	sprintf(print, "Network1 Output Size: %d\n", outputSize);
	ocall_print_buffer(print);
}


Network::~Network()
{
		delete m_Readdata;
    delete m_Layers_bn;
}


void Network::Forward()
{
	ocall_print_time("Inference 1 Start");
	
	ocall_print_time("Communication 0 Start");
	m_Layers_bn->forward(m_Readdata->ReadInput(1));
	ocall_print_time("Communication 0 End");
	
	m_pfOutput = m_Layers_bn->GetOutput();
	
	ocall_print_time("Inference 1 End");
	
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, pubkey, netiv);

	ocall_print_time("Communication 1 Start");
	
	size_t bytesToWrite = m_Layers_bn->GetOutputSize() * sizeof(float);
	
	uint8_t inputBuf[2032];
	size_t offset = 0;
	
	while(bytesToWrite > 0) 
	{
		//write 2032 bytes or whatever is left
		size_t chunkSize = (bytesToWrite > 2032) ? 2032 : bytesToWrite;
		
		memcpy(inputBuf, reinterpret_cast<uint8_t*>(m_pfOutput) + offset, chunkSize);
		
		//apply padding
		size_t length = sizeof(inputBuf);
		uint8_t* buffer = (uint8_t*)malloc(length + AES_BLOCKLEN);
		memcpy(buffer, inputBuf, length);
		size_t padded_length = length;
		pad_buffer(buffer, &padded_length);
		
		//encrypt
		AES_CBC_encrypt_buffer(&ctx, buffer, padded_length);
		ocall_print_string((char*)buffer);
		
		offset += chunkSize;
		bytesToWrite -= chunkSize;
		free(buffer);
	}
	
	ocall_print_time("Communication 1 End");
	
	return;
}
