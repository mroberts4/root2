#include "utils2.h"
#include "Network2.h"
#include <string>
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "string.h"
#include <time.h>
#include <iostream>

#include "eapp_utils.h"
#include "edge_call.h"
#include "syscall.h"
#include "crypto.h"
#include "aes.hpp"

uint8_t key[AES_KEYLEN] = {
			(uint8_t)0x1a, (uint8_t)0x2b, (uint8_t)0x3b, (uint8_t)0x4d,
			(uint8_t)0x5e, (uint8_t)0x6f, (uint8_t)0x71, (uint8_t)0x82,
			(uint8_t)0x93, (uint8_t)0x14, (uint8_t)0x25, (uint8_t)0x36,
			(uint8_t)0x47, (uint8_t)0x58, (uint8_t)0x69, (uint8_t)0x7a 
		};
		
uint8_t iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

#define OCALL_GET_STRING_12 2

float Accuracy_1(float *pfPred, int *pnLab, int nclass)
{
    float flag = 0.0;
    for (int i = 0; i < nclass; i++)
    {
        //cout << pfPred[i] << ':' << pnLab[i] << endl;
        if (pfPred[i] >=0.5 && pnLab[i] == 1)
            flag = 1.0;
    }
    return flag;
}

float Accuracy_all(float *pfPred, int *pnLab, int nclass)
{
    float flag = 0.0, nSum = 0.0;
    int *pnPred = new int[nclass];
    for (int i = 0; i < nclass; i++)
    {
        pnPred[i] = 0;
        if (pfPred[i] >=0.5)
            pnPred[i] = 1;

        if (pnPred[i] == pnLab[i])
            nSum += 1;
    }

    if (nSum == 12.0)
        flag = 1.0;

    delete[] pnPred;
    return flag;
}

void test()
{
  Network network;
  size_t inputSize = 275968;
  
  struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, key, iv);

	float* inputData = (float*)malloc(inputSize * sizeof(float));
	if(inputData == NULL)
	{
		fprintf(stderr, "Mem alloc failed\n");
		return;
	}
	memset(inputData, 0, inputSize * sizeof(float));
	
	size_t index = 0;
	
	while(index < inputSize)
	{
		uint8_t nonce[2048];
		size_t nonceLen = 2048;
		struct edge_data retdata;
		ocall(OCALL_GET_STRING_12, NULL, 0, &retdata, sizeof(struct edge_data));
		if(retdata.size > 2048)
			retdata.size = 2048;
		memset(nonce, 0, sizeof(nonce));
		copy_from_shared(nonce, retdata.offset, retdata.size);
		
		//decrypt
		AES_CBC_decrypt_buffer(&ctx, (uint8_t*)nonce, 2048);
		remove_padding((uint8_t*)nonce, &nonceLen);
		
		for(size_t i = 0; i < nonceLen; i += sizeof(float))
		{
			if(index < inputSize)
			{
				memcpy(&inputData[index], &nonce[i], sizeof(float)); 
				index++;
			}
			else
				break;
		}
  }
  
	network.Forward(inputData);
	return;
}


