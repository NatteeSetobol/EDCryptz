#ifndef __Encryption__CPP__
#define __Encryption__CPP__

#include <time.h>
#include <stdint.h>
#include "aes.h"
#include "cutil.h"

const char SIGN[] = "EX";

struct en_header
{
    short signature;
    int size;
    int padding;
};

void EncryptFile(char *filename, uint8_t *key, char* output=NULL);
void DecryptFile(char *filename, uint8_t* key, char* output=NULL);
uint8_t* ANSIX923Padding(uint8_t* data, size_t length,size_t *newLength);

uint8_t* AESEncrypt(uint8_t* data, size_t length, uint8_t* key);
uint8_t* AESDecrypt(uint8_t* data,size_t length, uint8_t* key);

#endif