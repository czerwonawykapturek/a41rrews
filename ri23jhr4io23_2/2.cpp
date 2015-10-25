#include <cstdio>
#include <cstring>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "utils.cpp"

using namespace std;

void delData(char** c, int l)
{
    for(int i = 0; i < l; i++)
    {
		delete(c[i]);
    }
    delete(c);
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
		cout<<"Uzycie programu: [program] [plik kryptogramu]"<<endl;
		return 1;
    }

    char realKey[KEYLEN + 1];
    char *partialKey = "aff1ea59";
    int k = 0;

    char** cg;

    FILE* file = fopen(argv[1], "r");

    char buffer[LINELEN + 2];
    int charCount = 0;

    while(fgets(buffer, sizeof(buffer), file))
    {
		charCount++;
    }

    cg = new char*[charCount];
    fseek(file, 0, 0);

    while(fgets(buffer, sizeof(buffer), file))
    {
		buffer[strlen(buffer) - 1] = '\0';
		cg[k] = new char[LINELEN + 1];
		strcpy(cg[k], buffer);
		k++;
    }
    fclose(file);

    //char missKey[BYTESMAX + 1] = "a6765a1e";
    char missKey[BYTESMAX + 1];
    int positionInKey[BYTESMAX];

	for(int i = 0; i <= BYTESMAX; i++)
	{
		missKey[i] = '\0';
	}

    for(int i = 0; i < BYTESMAX; i++)
    {
		positionInKey[i] = 0;
    }

    // mozliwe znaki w kluczu
    char possibleChars[KEYCHARS] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    long long int iteration = 0;

    bool allKeysChecked = false;
    bool realKeyFound = false;

    while(!allKeysChecked)
    {
		iteration++; // licznik
    	for(int i = 0; i < KEYLEN; i++)
    	{
    		realKey[i] = 0;
		}

		// skladanie klucza z czesci brakujacej i znanej
		strcat(realKey, missKey);
		strcat(realKey, partialKey);
		
		if(iteration % 1000000 == 0)
		{
			cout<<"Badany klucz: "<<realKey<<" (nr: "<<iteration<<")"<<endl;
		}

		int states[STATES];

		for(int i = 0; i < STATES; i++)
		{
		    states[i] = i;
		}

		int p = 0;
		int q = 0;

		for(int i = 0; i < STATES; i++)
		{
		    p = (p + states[i] + realKey[i % KEYLEN]) % STATES;

		    int t = states[i];
		    states[i] = states[p];
		    states[p] = t;
		}

		p = 0;
		q = 0;

		int lp = 0;
		bool isKeyInvalid = false;

		while(!isKeyInvalid && (lp < charCount))
		{
		    p = (p + 1) % STATES;
		    q = (q + states[p]) % STATES;

		    int t = states[p];

		    states[p] = states[q];

		    states[q] = t;

		    int r = states[(states[p] + states[q]) % STATES];

		    int c = r ^ binaryToDecimal(cg[lp++]);

		    // znaleziono prawidlowy klucz

		    if(!checkSymbol(c))
		    {
		    	isKeyInvalid = true;
		    }
		}

		keySetNext(positionInKey, BYTESMAX - 1);

		for(int i = 0; i < BYTESMAX; i++)
		{
		    missKey[i] = possibleChars[positionInKey[i]];
		}

		if(!strcmp(missKey, "00000000"))
		{
			allKeysChecked = true;
		}

		if(!isKeyInvalid)
		{
			cout<<"Znaleziono prawidlowy klucz: "<<realKey<<endl;
			realKeyFound = true;
			break;
		}
    }

    // jesli znaleziono prawidlowy klucz -> odkodowanie wiadomosci

    if(realKeyFound)
    {
    	FILE* f = fopen("key.txt", "w");
    	fwrite(realKey, sizeof(char), KEYLEN+1, f);
    	fclose(f);
    	cout<<"Klucz zapisano w pliku key.txt"<<endl;
    	cout<<"Czy rozkodowac plik kryptogramu? [t] - tak : ";
    	char r;
    	cin>>r;
    	if(r == 't')
    	{
    		cout<<"Zdekodowana wiadomosc: "<<endl;
    		printDecoded(realKey, cg, charCount);
    	}
    }

    delData(cg, charCount);

    return 0;
}
