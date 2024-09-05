#include "Network3.h"
#include <vector>
#include <iostream>
#include "string.h"

#include "eapp_utils.h"
#include "edge_call.h"
#include "syscall.h"

#include <chrono>
#include <iomanip>

#ifndef OCALL_PRINT_TIME
#define OCALL_PRINT_TIME 5
#endif

#define OCALL_PRINT_BUFFER 9

extern "C" 
{
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
	ocall_print_time("Network Init 3 Start\n");

	ocall_print_buffer("Initializing Network 3...\n");

	// Fully Connected Layer
	m_Fclayer7 = new FcLayer(7, 720, 12);  // Reduced from 1024 to 720
	
	// Sigmoid Layer
	m_Sigmoidlayer8 = new SigmoidLayer(12);
	
	m_vcClass.push_back("室内");
	m_vcClass.push_back("人像");
	m_vcClass.push_back("LDR");
	m_vcClass.push_back("绿植");
	m_vcClass.push_back("商场");
	m_vcClass.push_back("沙滩");
	m_vcClass.push_back("逆光");
	m_vcClass.push_back("日落");
	m_vcClass.push_back("蓝天");
	m_vcClass.push_back("雪景");
	m_vcClass.push_back("夜景");
	m_vcClass.push_back("文本");
	
	ocall_print_buffer("Initializing Network 3 Done...\n");
	
	ocall_print_time("Network Init 3 End");
}


Network::~Network()
{
    delete m_Fclayer7;
    delete m_Sigmoidlayer8;
}


float* Network::Forward(float* input)
{
	ocall_print_time("Inference 3 Start");
	
	ocall_print_buffer("Getting output...\n");
	
  m_Fclayer7->forward(input);

  m_Sigmoidlayer8->forward(m_Fclayer7->GetOutput());

  float *pfOutput = m_Sigmoidlayer8->GetOutput();
	vector <int> argmax;
	vector <float> Max;

  int nOutputSize = m_Fclayer7->GetOutputSize();
	for (int i = 0; i<nOutputSize; i++)
	{
		if (pfOutput[i] > 0.5)
		{
			argmax.push_back(i);
			Max.push_back(pfOutput[i]);
		}
	}
	
	ocall_print_time("Communication 3 Start");
	for (int i = 0; i < argmax.size(); i++)
	{
		char print[256];
		snprintf(print, sizeof(print), "%10f: %d: %s\n", Max[i], argmax[i], m_vcClass[argmax[i]]);
		ocall_print_buffer(print);
	}
	ocall_print_time("Communication 3 End");
	
	ocall_print_time("Inference 3 End");
    
  return pfOutput;
}
