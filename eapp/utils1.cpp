#include "utils1.h"
#include "Network1.h"
#include <string>
#include <fstream>

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
		network.Forward();
 		return;
}



