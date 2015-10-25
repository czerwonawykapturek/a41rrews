#include <cstring>
#include <cstdio>
#include <iostream>
#include <cctype>
#include <cstdlib>
#include <cmath>
#include "utils.cpp"

using namespace std;

int main(int argc, char** argv)
{
    if(argc == 1)
    {
        cout<<"Uzycie: [program] [plik-kryptogram 1] ..."<<endl;
        return 1;
    }
    int k = 0;

    int n = argc-1;

    char*** cg = new char**[n];
    int* dimensions = new int[n];

    char** encKey;
    int maxcharsCount = 0;
    int i = 0;

    for(int u = 1; u < argc; u++)
    {
        i = 0;
        FILE *file = fopen(argv[u], "r");
        
        if(file == NULL)
        {
            cout<<"Blad podczas wczytywania pliku: < "<<argv[u]<<" >"<<endl;
            return 1;
        }

        char characterRead[LEN + 2]; // wczytany znak reprezentowany binarnie -> tablica znakow
        int charsCount = 0; //liczba znakow we wczytanym pliku

        while(fgets(characterRead, sizeof(characterRead), file))
        {
            charsCount++;
        }

        if(charsCount > maxcharsCount)
        {
            maxcharsCount = charsCount;
        }

        dimensions[k] = charsCount; //zapisujemy rozmiary kazdego kryptogramu
        cg[k] = new char*[charsCount]; //tablica wszystkich kryptogramow
        fseek(file,0,0);

        while(fgets(characterRead, sizeof(characterRead), file))
        {
            cg[k][i] = new char[LEN + 1];
            characterRead[strlen(characterRead) - 1] = '\0';
            strcpy(cg[k][i], characterRead);
            i++;
        }
        fclose(file);
        k++;
    }
	
    // klucz
    encKey = new char*[maxcharsCount];

    for(int i = 0; i < maxcharsCount; i++)
    {
        encKey[i] = new char[LEN+1];
        for(int j = 0; j < LEN+1; j++)
        {
            encKey[i][j] = '\0';
        }
    }

    for(int i = 0; i < n-1; i++)
    {
        for(int j = i+1; j < n; j++)
        {
            int cr1 = i; //nr kryptogramu

            if(dimensions[i] > dimensions[j]) // wybierz krotszy kryptogram jako baze
            {
                cr1 = j;
            }

            int cr1Length = dimensions[cr1]; // dlugosc kryptogramu w znakach
            int d = 0;
            for(d = 0; d < cr1Length; d++) // przejscie po znakach
            {
                char xor_op[LEN + 1]; // wynik operacji XOR
                xorBits(cg[i][d], cg[j][d], xor_op); // operacja XOR na znakach kryptogramow i oraz j

                int xorResult = binaryToDecimal(xor_op);

                if(isalpha(xorResult)) // sprawdz czy wynik XORa jest znakiem alfabetycznym
                {
                    char vEncKey[2][LEN + 1];
                    char *spaceInBinary = "00100000";

                    xorBits(cg[i][d], spaceInBinary, vEncKey[0]);
                    xorBits(cg[j][d], spaceInBinary, vEncKey[1]);

                    int vEncKeyIsValid[2] = {1, 1};

                    for(int c = 0; c < n; c++) // iteruj po kazdym kryptogramie
                    {
                        if(d < dimensions[c])
                        {
                            int q = 0;
                            for(q = 0; q < 2; q++)
                            {
                                char m[LEN + 1];
                                xorBits(cg[c][d], vEncKey[q], m);

                                int s = binaryToDecimal(m);

                                if(!checkSymbol(s)) // sprawdz inne znaki
                                {
                                    vEncKeyIsValid[q] = 0;
                                }
                            }
                        }
                    }

                    if(vEncKeyIsValid[0] != vEncKeyIsValid[1])
                    {
                        int keyId = 0;
                        if(vEncKeyIsValid[1] == 1)
                        {
                            keyId = 1;
                        }
                        strcpy(encKey[d], vEncKey[keyId]);
                    }
                }
            }
        }
    }

    for(int x = 0; x < n; x++)
    {
        cout<<"Kryptogram: ["<<x<<"]"<<endl;

        int i = 0;
        for(i = 0; i < dimensions[x]; i++)
        {
            if(strlen(encKey[i]) > 0)
            {
                char xor_op[LEN + 1];
                xorBits(cg[x][i], encKey[i], xor_op); // odczyt kryptogramu przy pomocy klucza
                printf("%c", binaryToDecimal(xor_op));
            }
            else
            {
                cout<<"*";
            }
        }
        cout<<endl<<endl<<endl;
    }

    return 0;
}
