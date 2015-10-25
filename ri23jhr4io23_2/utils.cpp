#include <cstdio>
#include <cstring>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iostream>

#define KEYCHARS 16
#define STATES 256
#define LINELEN 8
#define KEYLEN 16
#define BYTESMAX 8

using namespace std;

int binaryToDecimal(char* binary)
{
    int x = 0;
    int result = 0;
    int len = strlen(binary);
	int i = 0;

    for(i = len - 1; i >= 0; i--)
    {
        if(binary[i] == '1')
        {
            result += (int) pow(2.0, (double) x);
        }
	   x++;
    }

    return result;
}

void keySetNext(int* ss, int i)
{
    ss[i]++;

    if(ss[i] == KEYCHARS)
    {
	   ss[i] = 0;

        if(i > 0)
        {
            keySetNext(ss, i - 1);
        }
    }
}

int checkSymbol(char s)
{
    if(isalpha(s) || isdigit(s))
    {
		return 1;
	}

    char chars[] =  {' ', '\'', '.', '"', ',', ':'};

    for(int i = 0; i < 6; i++)
    {
        if(chars[i] == s)
        {
            return 1;
        }
    }

    return 0;
}

void printDecoded(char* key, char** crypt, int lns)
{
    int ss[STATES];

    for(int i = 0; i < STATES; i++)
    {
        ss[i] = i;
    }

    int p = 0;
    int q = 0;

    for(int i = 0; i < STATES; i++)
    {
        p = (p + ss[i] + key[i % KEYLEN]) % STATES;
        int rr = ss[i];
        ss[i] = ss[p];
        ss[p] = rr;
    }

    p = 0;
    q = 0;

    for(int i = 0; i<lns; i++)
    {
        p = (p + 1) % STATES;
        q = (q + ss[p]) % STATES;

        int t = ss[p];
        ss[p] = ss[q];
        ss[q] = t;

        int asd = ss[(ss[p] + ss[q]) % STATES];
        int character = asd ^ binaryToDecimal(crypt[i]);
        printf("%c", character);
    }
}