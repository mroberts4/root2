#include "crypto.h"
#include "string.h"

void xorEnDecrypt(char* input, const char* key, size_t keysize)
{
	char* strPtr = input;
	size_t inputLength = strlen(strPtr);
	
	for(size_t i = 0; i < inputLength; ++i)
	{
		input[i] = input[i] ^ key[i % keysize];
	}
	
	return;
}

void pad_buffer(uint8_t* buf, size_t* length) 
{
	size_t padding_needed = AES_BLOCKLEN - (*length % AES_BLOCKLEN);
	for(size_t i = *length; i < *length + padding_needed; i++)
	{
		buf[i] = (int8_t)padding_needed;
	}
	
	*length += padding_needed;
}

void remove_padding(uint8_t* buf, size_t* length)
{
	uint8_t padding_len = buf[*length - 1];
	if(padding_len > 0 && padding_len <= AES_BLOCKLEN)
	{
		*length -= padding_len;
	}
}

