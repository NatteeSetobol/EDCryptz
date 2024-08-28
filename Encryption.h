#ifndef __Encryption__CPP__
#define __Encryption__CPP__

#include <time.h>
#include <stdint.h>
#include "ErrorHandle.h"
#include "aes.h"
#include "cutil.h"

const char SIGN[] = "EX";

struct en_header
{
    short signature;
    int size; //encyption size of the file
    int padding;
    float version;
    int filenameLen;
};


uint8_t *DecryptString(uint8_t* filename, uint8_t *key, size_t fileLen);
uint8_t *EncryptString(char* filename, uint8_t *key, size_t *encryptionSize);
void EncryptFile(char *filename, uint8_t *key, char* output=NULL);
void DecryptFile(char *filename, uint8_t* key, char* output=NULL);
uint8_t* ANSIX923Padding(uint8_t* data, size_t length,size_t *newLength);

uint8_t* AESEncrypt(uint8_t* data, size_t length, uint8_t* key);
uint8_t* AESDecrypt(uint8_t* data,size_t length, uint8_t* key);

#endif