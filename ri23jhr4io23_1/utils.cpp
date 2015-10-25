#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cstring>

#define LEN 8

using namespace std;

int checkSymbol(char symbol)
{
    if(isalpha(symbol))
    {
        return 1;
    }

    char otherChars[] = {',',' ','\'',':','"','.'};
    int i = 0;
    for(i = 0; i < 6; i++)
    {
        if(otherChars[i] == symbol)
        {
            return 1;
        }
    }

    return 0;
}

void xorBits(char* bits1, char* bits2, char* result)
{
    int i = 0;
    for(i = 0; i < LEN; i++)
    {
        if(bits1[i] == bits2[i])
        {
            result[i] = '0';
        }
        else
        {
            result[i] = '1';
        }
    }

    result[LEN] = '\0';
}

void decimalToBinary(int decimal, char* binResult)
{
    int i = 0;
    for(i = 0; i < LEN; i++)
    {
        binResult[i] = '0';
    }
    binResult[LEN] = '\0';
    i = LEN-1;

    while(decimal > 0)
    {
        int res = decimal % 2;
        decimal = decimal / 2;

        binResult[i--] = 48+res;
    }
}

int binaryToDecimal(char* binary)
{
    int len = strlen(binary);
    int decimal = 0;
    int n = 0;
    int i = 0;

    for(i = len - 1; i >= 0; i--)
    {
        if(binary[i] == '1')
        {
            decimal = decimal + (int) pow(2.0, (double) n);
        }
        n++;
    }

    return decimal;
}