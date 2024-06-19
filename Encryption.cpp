#include "cutil.h"
#include "Encryption.h"

#define KEY_SIZE 16
#define BLOCK_SIZE 16

uint8_t* ANSIX923Padding(uint8_t* data, size_t length,size_t *newLength)
{
    uint8_t len = 0;
    uint8_t *newData = NULL;

    len = AES_BLOCKLEN - length % AES_BLOCKLEN;

    if (len == length)
    {
        newData = (uint8_t*) Alloc(length*sizeof(uint8_t ));
        memcpy(newData,data,length);
        //newData = data;
    } else {
        size_t newsize = 0;
        newsize = length + len; 
        if (newsize >= 16)
        {
            newData = (uint8_t*) Alloc(newsize*sizeof(uint8_t ));
            memcpy(newData,data,length);

            *newLength = newsize;

 //           if (data)
  //          {
   //             free(data);
    //            data = NULL;
     //       }
        
        }
    }

    return newData;
}

uint8_t* AESEncrypt(uint8_t* data, size_t length, uint8_t* key)
{
    size_t num_blocks = 0;
    uint8_t *newData = NULL;
    size_t newLength = 0;
    AES_ctx ctx;
    
    AES_init_ctx(&ctx,key);


    newData =  ANSIX923Padding(data, length, &newLength);

    #ifdef DEBUG
        printf("New Data: %i\n", newLength);
    #endif

    num_blocks = newLength / BLOCK_SIZE;

    #ifdef DEBUG
        printf("number of blocks: %i\n", num_blocks);
    #endif

    FILE *newFile = NULL;

 //   newFile = fopen("test_encry.txt", "wb+");
    for (size_t i=0; i< num_blocks; i++)
    {
        AES_ECB_encrypt(&ctx,newData + (i * BLOCK_SIZE));

   //     fwrite(newData + (i * BLOCK_SIZE), 1,BLOCK_SIZE,newFile);

        #ifdef DEBUG
            PrintHex(newData + (i * BLOCK_SIZE), BLOCK_SIZE);
        #endif
        //AES_ECB_encrypt(data + (i * BLOCK_SIZE), key, data+(i * BLOCK_SIZE));
    }

    //fclose(newFile);

    return newData;
}

uint8_t *AESDecrypt(uint8_t* data,size_t length, uint8_t* key)
{
    uint8_t* newData = NULL;
    size_t num_blocks=0;
    
    AES_ctx ctx;
    AES_init_ctx(&ctx,key);

    num_blocks = length / BLOCK_SIZE;

    printf("number of blocks: %i\n", num_blocks);

    for (size_t i = 0; i < num_blocks; i++)
    {
        AES_ECB_decrypt(&ctx,data + (i * BLOCK_SIZE));

        #ifdef DEBUG
           PrintHex(data + (i * BLOCK_SIZE), BLOCK_SIZE);
        #endif
    }

    return data;
}


struct en_header CreateHeader(int fileLength, int padding)
{
    struct en_header header = {};

    header.signature = 0xEB;
    header.size = fileLength;
    header.padding = padding;

    return header;
}

void EncryptFile(char *filename, uint8_t *key,char *output)
{
    uint8_t *fileWithPadding= NULL; 
    uint8_t *fileData = NULL; 
    size_t fileLen = 0;
    size_t newLength = 0;
    AES_ctx ctx;
    size_t blocks = 0;
    struct en_header header={};
    char *id = NULL;

    AES_init_ctx(&ctx,key);

    fileData = ReadFromFile(filename, &fileLen);
    
    fileWithPadding =  ANSIX923Padding(fileData, fileLen, &newLength);

    blocks = newLength / BLOCK_SIZE;

    for (size_t i=0; i< blocks; i++)
    {
        AES_ECB_encrypt(&ctx,fileWithPadding + (i * BLOCK_SIZE));
    }

    header = CreateHeader(fileLen, newLength-fileLen);

    if (output == NULL)
    {
        output = filename;
    }

    WriteToFile(output, (uint8_t*) &header, sizeof(struct en_header),"w");
    WriteToFile(output, fileWithPadding, newLength,"a");


    memset(fileData,0x00,fileLen);
    memset(fileWithPadding,0x00,newLength);

    if (fileData)
    {
        free(fileData);
        fileData = NULL;
    }
    if (fileWithPadding)
    {
        free(fileWithPadding);
        fileWithPadding= NULL;
    }
 //   PrintHex(fileWithPadding, newLength);
}

void DecryptFile(char *filename, uint8_t* key, char *output)
{
    size_t num_blocks=0;
    uint8_t *fileData = NULL; 
    size_t fileLen=0;
    struct en_header *header=NULL;
    uint8_t* encryptedData = NULL; 
    size_t encryptedDataLen = 0;
    
    AES_ctx ctx;
    AES_init_ctx(&ctx,key);

    fileData = ReadFromFile(filename, &fileLen);

    header = (struct en_header*)  fileData;

    encryptedData = fileData+sizeof(struct en_header);

    encryptedDataLen = header->size + header->padding;

    num_blocks = encryptedDataLen / BLOCK_SIZE;
    for (size_t i = 0; i < num_blocks; i++)
    {
        AES_ECB_decrypt(&ctx,encryptedData + (i * BLOCK_SIZE));
    }
    if (output == NULL) 
    {
        output = filename;
    }  
    WriteToFile(output, encryptedData, header->size,"wb");
}