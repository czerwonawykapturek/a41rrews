#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <ao/ao.h>
#include <mpg123.h>
#include "openssl.h"

using namespace std;

int main (int argc, char *argv[])
{
	char configFilename[] = "config";
	char zad1Filename[] = "zad1";
	unsigned char configKey[] = "15245378901234567890123456789012";;
	unsigned char ivConf[18] = "01234567890123456";
	
	if(access(zad1Filename, F_OK) == -1)
	{
	  printf("Nie znaleziono programu zad1.");
	  printf("\nUpewnij sie ze program zad1 znajduje sie w tym samym katalogu co program zad2\n");
	  return 1;
	}
	
	if(argc == 2)
	{
	  if(strcmp(argv[1], "install") == 0)
	  {
	    // instalacja programu -> tworzenie pliku config i szyfrowanie go
	    FILE* fConfig = fopen(configFilename, "w");
	    
	    char output[255];
	    strcat(output, "keystore ");
	    strcat(output, (char*)ivConf);
	    strcat(output, " ");
	    strcat(output, "1234");
	    
	    unsigned char* encOutput = (unsigned char*)malloc(128 * sizeof(unsigned char));
	    int encrypted_len = encrypt((unsigned char*)output, strlen(output), configKey, ivConf, encOutput);
	    fwrite(encOutput, sizeof(char), encrypted_len, fConfig);
	    
	    fclose(fConfig);	    
	    printf("Program zainstalowano.\n");
	  }
	  else
	  {
	    printf("Nieprawidlowe polecenie\n");
	  }
	  return 0;
	}
	
	// wczytywanie pliku konfiguracyjnego

	unsigned char key[33];
	unsigned char iv[18];

	
	// te opcje zostana odczytane z pliku config
	char keystoreFilename[255];// = "keystore";
	char inputFilename[255];
	char outputFilename[] = "temp_file"; // tymczasowy plik z odszyfrowanym utworem - do usuniecia po zakonczeniu odtwarzania
	char id[255];// = "01234567890123456"; // id odczytywane z configa
	char mode[] = "d"; // opcja deszyfrowania bo chcemy odtwarzac pliki
	char pin[255]; // pin - haslo dostepu do programu
	
	if(access(configFilename, F_OK) == -1)
	{
	    printf("Brak pliku konfiguracyjnego: %s\n", configFilename);
	    printf("Zainstaluj program poleceniem ./zad2 install");
	    return 1;	  
	}
	
	FILE* fConfig = fopen(configFilename, "r");
	
	unsigned char* buffer = (unsigned char*)malloc(128 * sizeof(unsigned char));
	int read = fread((char*)buffer, sizeof(char), 128, fConfig);
	
	// odszyfrowanie pliku konfiguracyjnego
	unsigned char* decryptedBuffer = (unsigned char*)malloc(128 * sizeof(unsigned char));
	int decrypted_len = decrypt(buffer, read, configKey, ivConf, decryptedBuffer);
	decryptedBuffer[decrypted_len] = '\0';
	
	sscanf((char*)decryptedBuffer, "%s %s %s", keystoreFilename, id, pin);
	
	// sprawdzanie hasla uzytkownika
	
	char haslo[255];
	printf("Wprowadz haslo dostepu do programu (PIN): ");
	scanf("%s", haslo);
	while(strcmp(haslo, pin) != 0)
	{
	  printf("Bledne haslo. Wprowadz ponownie: ");
	  scanf("%s", haslo);
	}
	
	// otwieranie zaszyfrowanego pliku muzycznego
	
	char songFilename[255];
	
	printf("Podaj nazwe zaszyfrowanego pliku, ktory chcesz otworzyc: ");
	scanf("%s", songFilename);
	
	// sprawdzanie czy plik o danej nazwie istnieje
	
	while(access(songFilename, F_OK) == -1)
	{
	  printf("Nie mozna otworzyc pliku. Wprowadz nazwe ponownie: ");
	  scanf("%s", songFilename);
	}
	
	// tutaj otwieramy i deszyfrujemy plik za pomoca programu z zadania 1
	
	char cmd[255];
	
	strcat(cmd, "./zad1 ");
	strcat(cmd, keystoreFilename);
	strcat(cmd, " ");
	strcat(cmd, id);
	strcat(cmd, " ");
	strcat(cmd, mode);
	strcat(cmd, " ");
	strcat(cmd, songFilename);
	strcat(cmd, " ");
	strcat(cmd, outputFilename);
	
	// wywolanie programu z zadania 1
	
	system(cmd);
	
	if(access(outputFilename, F_OK) == -1)
	{
	  printf("Program zad1 nie utworzyl pliku tymczasowego. Sprobuj ponownie.\n");
	  return 1;
	}
	
	// wszystko w porzadku - otwieramy plik tymczasowy!
	
	unsigned char* data;
	size_t bufSize, fin;
	int driverId, error;
	mpg123_handle* songHandler;
	ao_device *dev;
	
	// inicjalizacja libao
	
	ao_initialize();
	mpg123_init();
	
	driverId = ao_default_driver_id();
	songHandler = mpg123_new(NULL, &error);
	bufSize = mpg123_outblock(songHandler);
	
	data = (unsigned char*)malloc(bufSize * sizeof(unsigned char));
	
	
	mpg123_open(songHandler, outputFilename);
	
	ao_sample_format format;
	int channels, encoding;
	long rate;
	mpg123_getformat(songHandler, &rate, &channels, &encoding);
	
	format.channels = channels;
	format.matrix = 0;
	format.byte_format = AO_FMT_NATIVE;
	format.bits = 8 * mpg123_encsize(encoding);
	format.rate = rate;
	
	dev = ao_open_live(driverId, &format, NULL);
	
	while(mpg123_read(songHandler, data, bufSize, &fin) == MPG123_OK)
	{
	  ao_play(dev, (char*)data, fin);
	}
	
	remove(outputFilename);
	ao_close(dev);
	mpg123_close(songHandler);
	mpg123_delete(songHandler);
	mpg123_exit();
	ao_shutdown();
	
	free(data);
	
	return 0;
}







































