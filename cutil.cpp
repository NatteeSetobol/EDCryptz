#include "cutil.h"

const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void *Alloc(size_t blockSize)
{
    void *memory = NULL;

    memory = malloc(blockSize+10);

    memset(memory, '\0', blockSize+10);

    return memory;  
}

uint8_t *ReadFromFile(const char* filename, size_t* length)
{
    FILE* file=NULL;
    uint8_t *data=NULL;

    file = fopen(filename,"rb");

    if (!file)
    {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    fseek(file,0,SEEK_SET);

    data = (uint8_t*) Alloc(*length+1);

    if (data) 
    {
        fread(data,1,*length,file);
        fclose(file);

        return data;
    }   

    return NULL;   
}

bool WriteToFile(const char* filename, uint8_t *data, size_t length, char* mode)
{
    FILE* file=NULL;

    file = fopen(filename,mode);

    if (file)
    {
      fwrite(data,1, length,file);
    } else {
      return false;
    }

    fclose(file);

    return true;
}

uint8_t toByte(bool b[8]) {
  uint8_t c = 0;
  for (int i = 0; i < 8; i++)
    if (b[i]) c |= 1 << i;
  return c;
}

void bernoulli(bool* dist, int N, float p, int& x) {
  int i = 0;
  int ct = 0;
  int cf = 0;
  while (i < N && ct < N*p && cf < N-N*p) {
    dist[i] = rand() % 2 == 1;
    if (dist[i])
      ct++;
    else
      cf++;
    i++;
  }
  if (p == 0 || p == 1)
    x = N - 1;
  else
    x = i - 1;
  while (i < N && ct < N*p) {
    dist[i] = true;
    ct++;
    i++;
  }
  while (i < N && cf < N-N*p) {
    dist[i] = false;
    cf++;
    i++;
  }
}

uint8_t* advandedRNG(char* &id, unsigned int &len, unsigned int seed) 
{
  uint8_t bytes[4];
  bytes[0] = (seed >> 24) & 0xFF;
  bytes[1] = (seed >> 16) & 0xFF;
  bytes[2] = (seed >> 8) & 0xFF;
  bytes[3] = seed & 0xFF;
  id = EncodeBase64(bytes, 4, len);

  srand(seed);
  uint8_t* key = new uint8_t[32];
  bool* prob = new bool[256];
  int x = 0, x_rel = 0, N = 256;
  float q;
  do {
    do {
      q = rand() % (N + 1);
    } while ((q == 0 || q == N) && N != 1);
    float p = q / N; // p in [0..1]
    bernoulli(&prob[x_rel], N, p, x);
    N -= x + 1;
    x_rel += x + 1;
  } while (x_rel != 256);

  for (int i = 0; i < 32; i++) {
    bool* pos = &prob[8 * i];
    key[i] = toByte(pos);
  }
  delete[] prob;
  return key;
}


char* EncodeBase64(const uint8_t* src, unsigned int len, unsigned int &out_len) {
  char* out = new char[len * 4 / 3 + 4];
  unsigned int i = 0, j = 0;
  while (len - i >= 3) {
    out[j] = base64_table[src[i] >> 2];
    out[j+1] = base64_table[((src[i] & 0x03) << 4) | (src[i+1] >> 4)];
    out[j+2] = base64_table[((src[i+1] & 0x0f) << 2) | (src[i+2] >> 6)];
    out[j+3] = base64_table[src[i+2] & 0x3f];
    i += 3;
    j += 4;
  }
  if (i != len) {
    out[j] = base64_table[src[i] >> 2];
    if (len - i == 2) {
      out[j+1] = base64_table[((src[i] & 0x03) << 4) | (src[i+1] >> 4)];
      out[j+2] = base64_table[(src[i+1] & 0x0f) << 2];
    } else {
      out[j+1] = base64_table[(src[i] & 0x03) << 4];
      out[j+2] = '=';
    }
    out[j+3] = '=';
    j += 4;
  }
  out_len = j;
  return out;
}

uint8_t* DecodeBase64(const char* src, unsigned int len, unsigned int &out_len) {
  uint8_t dtable[256];
  memset(dtable, 0x80, 256);
  for (unsigned int i = 0; i < 64; i++)
    dtable[(int)base64_table[i]] = (uint8_t)i;
  dtable['='] = 0;
  unsigned int pad = 0;
  uint8_t* out = new uint8_t[len / 4 * 3 + 1];
  unsigned int i = 0, j = 0;
  while (i < len) {
    out[j] = (dtable[(int)src[i]] << 2) | (dtable[(int)src[i+1]] >> 4);
	out[j+1] = (dtable[(int)src[i+1]] << 4) | (dtable[(int)src[i+2]] >> 2);
	out[j+2] = (dtable[(int)src[i+2]] << 6) | dtable[(int)src[i+3]];
	i += 4;
    j += 3;
  }
  if (src[i-1] == '=')
    pad++;
  if (src[len-2] == '=')
    pad++;
  out_len = j - pad;
  out[out_len] = '\0';
  return out;
}

void PrintBase64(const char* src, unsigned int len) {
  for (unsigned int i = 0; i < len; i++) {
  	printf("%c", src[i]);
  	if ((i+1) % 64 == 0) printf("\n");
  }
  printf("\n");
}

void PrintHex(const uint8_t* str, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    if (i > 0 && i % 16 == 0)
      printf("\n");
    printf("%.2X ", str[i]);
  }
  printf("\n");
}

uint8_t chartohex(char c) {
  //assert(isxdigit(c));
  uint8_t res;
  if (c >= '0' && c <= '9')
    res = c - '0';
  else if (c >= 'a' && c <= 'f')
    res = c - 'a' + 10;
  else //if (c >= 'A' && c <= 'F')
    res = c - 'A' + 10;
  return res;
}

bool StrCmp(char* source, char* compareString)
{
	int i = 0;
	int sourceLen = 0;
	bool results = true;
	if (strlen(source) != strlen(compareString)) results = false;

	sourceLen = strlen(source);

	for (i = 0; i < sourceLen; i++)
	{
		if (source[i] != compareString[i])
		{
			results = false;
			break;
		}
	}

	return results;
}

uint8_t* ToHexStr(const uint8_t* str, uint8_t len)
{
  uint8_t *encryptedString = (uint8_t*) Alloc(len*2);
  int pos=0;

  for (uint8_t i = 0; i < len; i++) {
    char temp[25] = {};
    int tempLen = 0;


    sprintf(temp, "%.2x", str[i]);
    tempLen = strlen(temp);

    memcpy(encryptedString+pos,temp,tempLen);
    pos +=  tempLen;
  }
  printf("\n");
 
  return encryptedString;
}
uint8_t *ToStrHex(uint8_t* str)
{
  uint8_t *hexData = NULL;
  int  strLen = 0;
  int pos = 0;
  
  strLen = strlen((char*) str);

  hexData = (uint8_t*) Alloc(strLen);

  for (int i=0; i < strLen;i+=2)
  {
      uint8_t hexLeft = 0;
      uint8_t hexRight= 0;
      uint8_t combined[1] = {};

      hexLeft = chartohex(str[i]);
      hexRight= chartohex(str[i+1]);

      combined[0] |= hexLeft;
      combined[0] = combined[0] <<  4 ;
      combined[0] |=hexRight;

      memcpy(hexData+pos,combined,sizeof(uint8_t));

      pos++;
  }

  printf("\n");

  return hexData;
}

uint8_t *GetFilenameFromPath(char* filename)
{
    uint8_t *newStringName = NULL;
    uint8_t *decryptedDir = NULL;
    int slashPos=0;
    int filenameLen = 0;
    int findSlashLen = 0;
    int newFilenameLen = 0;

    filenameLen = strlen(filename);

    findSlashLen = filenameLen;

    if (filename[filenameLen] == '/')
    {
        findSlashLen--;
    }

    for (int i=findSlashLen;i > -1;i--)
    {
        if (filename[i] == '/')
        {
            slashPos = i;
            break;
        }
    }

    newStringName = (uint8_t*) Alloc(filenameLen);
    newFilenameLen = findSlashLen - slashPos;

    /*
        /home/myhomedir/code/123421/test 
        size = 32
        slash_pos = 28
        32-28 = 4
    */
    if (findSlashLen > 0)
    {
        memcpy(newStringName,filename+slashPos+1, newFilenameLen);
    }

    return newStringName;
}

bool FindChar(char* haystack, char needle)
{
  int haystackLen = 0;

  haystackLen = strlen(haystack);

  for (int i=0;i<haystackLen;i++)
  {
    if (haystack[i] == needle)
    {
        return true;
    }
  }

  return false;
}