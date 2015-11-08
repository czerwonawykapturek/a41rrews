#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "openssl.h"
#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char *argv[])
{
	char keyPass[] = "pwr";
  
	if(argc != 6)
	{
		printf("Zla liczba parametrow. Parametry: keystore_file key_pass [e]ncrypt/[d]crypt input_file output_file\n");
		return 1;
	}

	//wczytujemy klucz

	unsigned char key[33];
	unsigned char iv[18];
	
	unsigned char keyKey[] = "10242567810124455062748628103434";

	char keyBuff[33];
	char ivBuff[18];
	
	FILE* fKey = fopen(argv[1], "r");
	FILE* fInput;
	FILE* fOutput;

	strcpy(ivBuff, argv[2]);

	if(fKey == NULL)
	{
	    printf("Problem podczas wczytywania pliku: %s\n", argv[1]);
	    return 1;
	}

	fInput = fopen(argv[4], "rb");

	if(fInput == NULL)
	{
	    printf("Problem podczas wczytywania pliku: %s\n", argv[4]);
	    return 1;
	}

	fOutput = fopen(argv[5], "wb");

	if(fOutput == NULL)
	{
	    printf("Problem podczas wczytywania pliku: %s\n", argv[5]);
	    return 1;
	}

	// wczytujemy zaszyfrowany klucz
	fread((char*)keyBuff, sizeof(char), 33, fKey);
	
	printf("Podaj haslo do klucza: ");
	char pass[255];
	scanf("%s", pass);
	while(strcmp(pass, keyPass) != 0)
	{
	  printf("Bledne haslo. Wprowadz ponownie: ");
	  scanf("%s", pass);
	}
	
	int key_len = decrypt((unsigned char*)keyBuff, 33, keyKey, (unsigned char*)pass, (unsigned char*)key);
	
	//memcpy(key, keyBuff, 33);
	memcpy(iv, ivBuff, 18);

	//czytanie pliku do zaszyfrowania/odszyfrowania

	unsigned char* buffer = (unsigned char*)malloc(128 * sizeof(unsigned char));
	unsigned char* encryptedBuffer = (unsigned char*)malloc(128 * sizeof(unsigned char));
	unsigned char* decryptedBuffer = (unsigned char*)malloc(128 * sizeof(unsigned char));

	int decrypted_len, encrypted_len;

	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	
	// szyfrowanie / deszyfrowanie
	int read = 0;
	int written = 0;
	
	do
	{
		// wczytaj blok danych i zaszyfruj albo odszyfruj go
		// po czym zapisz go do pliku wyjsciowego
	  
		read = fread((char*)buffer, sizeof(char), 128, fInput);
		if(argv[3][0] == 'e')
		{
			encrypted_len = encrypt(buffer,read, key, iv,encryptedBuffer);
			fwrite(encryptedBuffer, sizeof(char), encrypted_len, fOutput);
		}
		else if(argv[3][0] == 'd')
		{
			decrypted_len = decrypt(buffer,read, key, iv, decryptedBuffer);
			decryptedBuffer[decrypted_len] = '\0';
			fwrite(decryptedBuffer, sizeof(char), read, fOutput);
		}
 	}while(!feof(fInput));

	EVP_cleanup();
	ERR_free_strings();
	
	fclose(fKey);
	fclose(fInput);
	fclose(fOutput);

	return 0;
}