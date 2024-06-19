#ifndef __CUTIL_H__
#define __CUTIL_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint8_t ConvertToHex(char* hexChar);
bool WriteToFile(const char* filename, uint8_t *data, size_t length, char *mode);
uint8_t *ReadFromFile(const char* filename, size_t* length);
void *Alloc(size_t blockSize);
uint8_t toByte(bool b[8]);
uint8_t* advandedRNG(char* &id, unsigned int &len, unsigned int seed) ;
void bernoulli(bool* dist, int N, float p, int& x);
char* EncodeBase64(const uint8_t* src, unsigned int len, unsigned int &out_len);
uint8_t* DecodeBase64(const char* src, unsigned int len, unsigned int &out_len);
void PrintBase64(const char* src, unsigned int len);
void PrintHex(const uint8_t* str, uint8_t len);
uint8_t chartohex(char c);
bool StrCmp(char* source, char* compareString);
#endif