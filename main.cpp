#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdexcept>
#include "ErrorHandle.h"
#include "Encryption.h"

/*
    TO DO():
        * Add a version to the Encryption header
        * If not output is specified then the output should be tagged the original name + _e or _d
        * Encrypt and store filenames and directory in the header
*/

enum typeChoice {
    NONE,
    FILE_TYPE,
    DIRECTORY_TYPE
};

#define KEY_LEN 32

bool IsOnSkipList(char* filename);
bool EncryptDirectory(char *basePath, uint8_t *key, bool encrypt,char* output);
void ShowOptions();
void Logo();
char* CreatePath(char* basePath, char *filename);
void CheckKeyLength(char* key);

int main(int argc,char *args[] )
{
    bool gen = false;
    char *id = NULL;
    uint8_t *key = NULL;
    char *filename = NULL;
    unsigned int len = 16;
    bool encryptFile= false;
    bool choosenKey = false;
    enum typeChoice choice = NONE; 
    char *outputFile = NULL;
    bool edstring = false;
    char *edstringStr = NULL;
    char *outputfileTemp = NULL;
    /*j
    Get Options
    -e Encrypt file *required
    -d Decrypt file *required
    -k key *optional (needs to be in 16Bit/Hex format)
    -f file to Encrypt/Decrypt *required
    -o output of file(s)
        -if its a file then output to the output file
        -if not a file then
            1. make a new dir
            2. make the output the new dir + old filename
    */

    try
    {
       Logo();

        for (int i=1; i < argc;i++)
        {
            char *option = NULL;

            option = args[i];
            option = args[i];

            if (StrCmp(option, "-g"))
            {
                gen=true;
            } else
            if (StrCmp(option, "-e"))
            {
                encryptFile = true;
            } else
            if (StrCmp(option, "-d"))
            {
                encryptFile = false;
            } else
            if (i+1 < argc)
            {
                char *optionVal=NULL;

                optionVal = args[i+1];

                if (StrCmp(option, "-f"))
                {
                    struct stat info;

                    if (stat(optionVal, &info) == -1) ThrowError("Error File not found.");

                    if (S_ISDIR(info.st_mode))
                    {
                        choice = DIRECTORY_TYPE;
                        filename = optionVal;
                    } else {
                        choice = FILE_TYPE;
                        filename = optionVal;
                    }
                }

                if (StrCmp(option, "-s"))
                {
                    if (i+1 < argc)
                    {
                        edstringStr = args[i+1];
                        edstring = true;
                    }
                }
                if (StrCmp(option, "-o"))
                {
                    if (i+1 < argc)
                    {
                        outputfileTemp = args[i+1];

                    }
                }
                if (StrCmp(option, "-k"))
                {
                    /*
                    format A0B0C0E0F0010203040506070809A1A2
                    Check if the key length is 32
                    allocate space for new key unit8_t* = key uint8_t[32]
                    for loop by 2 to get hex and convert to uint_8
                    */

                    CheckKeyLength(optionVal);

                    int keyIndex = 0;

                    key =  new uint8_t[32];
                    
                    for (int i = 0;i<32;i+=2)
                    {
                        char extractedHex[3] = {};
                        uint8_t hexR='\0';
                        uint8_t hexL='\0';
                        uint8_t newKey = '\0';

                        extractedHex[0] = optionVal[i];
                        extractedHex[1] = optionVal[i+1];

                        hexR = chartohex(extractedHex[0]);
                        hexL = chartohex(extractedHex[1]);

                        newKey = ((hexR << 4) | hexL);
                        key[keyIndex] = newKey;
                        keyIndex++;                       
                        
                    }
                    
                    choosenKey=true;
                }
            } 
        }

        if (edstring == true)
        {

            uint8_t *encryptionBytes = NULL;

            if (encryptFile )
            {
                char *outputString=NULL;
                size_t encryptionSize = 0;

                if (choosenKey == false)
                {
                    unsigned int rngTime = 0;

                    rngTime = time(NULL) ^ clock();
                    key = advandedRNG(id, len, rngTime);
                } 
                
                encryptionBytes = EncryptString((char*)edstringStr, key, &encryptionSize);
                outputString = (char*) ToHexStr(encryptionBytes, encryptionSize);

                printf("%s\n", outputString);

                PrintHex(key, 16);
            } else {

                if (!choosenKey) ThrowError("Key must have a Length of 32.");

                uint8_t *decryptBytes = NULL;
                uint8_t *decryptString = NULL;

                decryptBytes = ToStrHex((uint8_t*) edstringStr );
                decryptString = DecryptString(decryptBytes, key, strlen(edstringStr)/2);

                printf("%s\n",decryptString );
            }

        } else
        if (gen==true)
        {
            unsigned int rngTime = 0;

            rngTime = time(NULL) ^ clock();
            key = advandedRNG(id, len, rngTime);
            
            PrintHex(key, 16);
            
        } else
        if (choice != NONE && CheckForError() == false)
        {
            if (encryptFile)
            {
                if (key == NULL)
                {
                    key = advandedRNG(id, len, time(NULL) ^ clock());
                }

                if (filename != NULL)
                {
                    if (choice == DIRECTORY_TYPE )
                    {
                        /*
                            NOTE(): If there is no out put indicated then we shall create a new directory with the same
                            name except tag _e at the end.
                        */

                        uint8_t *encryptionBytes = NULL;
                        size_t encryptionSize = 0;
                        uint8_t *unEncryptedFilename = NULL;

                        unEncryptedFilename = GetFilenameFromPath(filename);

                        encryptionBytes = EncryptString((char*)unEncryptedFilename, key, &encryptionSize);
                        outputFile = (char*) ToHexStr(encryptionBytes, encryptionSize);

                        mkdir(outputFile, 0755);
                        printf("[+] Encrypting Directory: %s to %s \n",unEncryptedFilename ,outputFile);

                        EncryptDirectory(filename,key, true, outputFile);
                        printf("[+] Encrypted Directory.");
                    } else {
                        if (outputfileTemp)
                        {

                            printf("[+] Encrypting file: %s\n", filename);
                            outputFile = (char*) Alloc(strlen(outputfileTemp));
                            strcpy(outputFile,outputfileTemp);

                            EncryptFile(filename, key, outputFile);
                            printf("[+] Encrypted file to %s\n", outputFile);
                        }
                    }

                    printf("\nYour key is: \n");
                    PrintHex(key, 16);

                } else {
                    ShowOptions();
                    printf("[-] File is required\n");
                } 
            } else 
            if (encryptFile == false)
            {
                if (key)
                {
                    if (filename != NULL)
                    {
                        if (choice == DIRECTORY_TYPE )
                        {
                            uint8_t *decryptBytes = NULL;
                            uint8_t *encyptedFilename = NULL;

                            encyptedFilename= GetFilenameFromPath(filename);

                            decryptBytes = ToStrHex(encyptedFilename);
                            outputFile = (char*)DecryptString(decryptBytes, key, strlen((char*)encyptedFilename) / 2 );

                            mkdir(outputFile, 0755);
                            printf("[+] Decrypting Directory: %s to %s\n",encyptedFilename ,outputFile);

                            if (EncryptDirectory(filename,key, false,outputFile))
                            {
                                printf("[+] Successfully decryped directory\n");
                            }
                        } else {
                            printf("[+] Decrypting file: %s\n", filename);
                            if (filename)
                            {
                                outputFile = (char*) Alloc(strlen(outputfileTemp));
                                strcpy(outputFile,outputfileTemp);
                                DecryptFile(filename, key, outputFile);
                                printf("[+] Decrypted file\n");
                            }
                        }
                    } else {
                        ShowOptions();
                        WriteError("[-] File is required\n");
                    }
                } else {
                    ShowOptions();
                    WriteError("[-] Key is required to Decrypt data.\n");
                }
            }
        } else {
            ShowOptions();
            WriteError("[-] No File specified.\n");
        }

    } catch(const std::runtime_error& e)
    {
        printf("[-] %s\n", e.what());
    }

    if (CheckForError())
    {
        PrintError();
    }

    ErrorClean();

    if (key)
    {
        delete[] key;
        key=NULL;
    }
    
    if (id)
    {
        delete[] id;
        id=NULL;
    }
}

bool EncryptDirectory(char *basePath, uint8_t *key, bool encrypt,char* output)
{
    DIR *dir = NULL;
    struct dirent *entry=NULL;
    bool success = true;
    
    if ((dir = opendir(basePath)))
    {
        while ((entry = readdir(dir)) != NULL) 
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                struct stat info= {};
                char path[1024] = {};

                snprintf(path, sizeof(path), "%s/%s", basePath, entry->d_name );

                if (stat(path, &info) == -1)
                {
                    success = false;
                } else {
                    if (S_ISDIR(info.st_mode))
                    {

                        char *folderPath = NULL;
                        
                        if (IsOnSkipList(entry->d_name))
                        {
                            printf("[!] Skipping %s\n", entry->d_name); 
                        } else { 
                            if (encrypt) {
                                uint8_t *encryptionBytes = NULL;
                                uint8_t *encryptionString= NULL;
                                size_t encryptionSize = 0;

                                encryptionBytes = EncryptString(entry->d_name, key, &encryptionSize);
                                encryptionString= ToHexStr(encryptionBytes, encryptionSize);

                                folderPath = CreatePath(output,(char*)encryptionString);

                                mkdir(folderPath,0755);
                            } else {
                                uint8_t *decryptBytes = NULL;
                                char *decryptString = NULL;

                                decryptBytes = ToStrHex((uint8_t*)  entry->d_name);
                                decryptString = (char*)DecryptString(decryptBytes, key, strlen(entry->d_name)/2);

                                folderPath = CreatePath(output,decryptString);

                                printf("Created Folder: %s\n",folderPath );
                                mkdir(folderPath,0755);
                            }

                            success = EncryptDirectory(path,key, encrypt, folderPath);
                        }                      
                    } else {
                        int fullPathLength=0;

                        fullPathLength = strlen(path) + strlen(entry->d_name) + 2;

                        if (fullPathLength > 2)
                        {
                            char *fullPath = NULL;
                            char *outputFile = NULL;

                            fullPath = CreatePath(basePath,entry->d_name);

                            if (encrypt)
                            {
                                uint8_t *encryptionBytes = NULL;
                                uint8_t *encryptionString = NULL;
                                size_t encryptionSize = 0;

                                encryptionBytes = EncryptString(entry->d_name, key, &encryptionSize);
                                encryptionString = ToHexStr(encryptionBytes, encryptionSize);

                                outputFile = CreatePath(output,(char*)encryptionString);

                                printf("Full Path: %s\n", fullPath);
                                printf("Output Path: %s\n", outputFile);
                                EncryptFile(fullPath, key,outputFile);
                            } else {

                                uint8_t *decryptBytes = NULL;
                                uint8_t *decryptString = NULL;

                                decryptBytes = ToStrHex((uint8_t*) entry->d_name );
                                decryptString = DecryptString(decryptBytes, key, strlen(entry->d_name)/2);

                                outputFile = CreatePath(output,(char*)decryptString);

                                DecryptFile(fullPath, key, outputFile);
                            }
                            
                            if (outputFile)
                            {
                                free(outputFile);
                                outputFile=NULL;
                            }

                            if (fullPath)
                            {
                                free(fullPath);
                                fullPath=NULL;
                            }
                            
                        }
                    }
                }
            }
        }
        closedir(dir);
    }
    return success;
}
void ShowOptions()
{
    printf("-e Encrypt File (required -e or -d)\n");
    printf("-d Decrypt File (required -e or -d)\n");
    printf("-f <File to encrypt or Decrypt> (required) \n");
    printf("-o <Output filename> (optional)\n");
    printf("-k <key> (optional)\n");
    printf("-g (optional) -- Generates a random 16bits key\n");
    printf("\n");
}


void Logo()
{
    printf(".,:::::::::::::-.    .,-::::: :::::::...-:.     ::-.::::::::::. :::::::::::::::::::::\n");
    printf(";;;;'''' ;;,   `';,,;;;'````' ;;;;``;;;;';;.   ;;;;' `;;;```.;;;;;;;;;;;'''''`````;;;\n");
    printf("[[cccc  `[[     [[[[[         [[[,/[[['  '[[,[[['    `]]nnn]]'      [[         .n[['\n");
    printf("$$\"\"\"\"   $$,    $$$$$         $$$$$$c      c$$\"       $$$\"\"         $$       ,$$P\n");
    printf("888oo,__ 888_,o8P'`88bo,__,o, 888b \"88bo,,8P\"`        888o          88,    ,888bo,_ \n");
    printf("\"\"\"\"YUMMMMMMMP\"`    \"YUMMMMMP\"MMMM   \"W\"mM\"           YMMMb         MMM     `\"\"*UMM\n");
    printf("\n");
}

char* CreatePath(char* basePath, char *filename)
{
    char* path=NULL;
    int pathLen = 0;

    pathLen = strlen(basePath) + strlen(filename)+10;

    path = (char*) Alloc(pathLen * sizeof(char));
    snprintf(path, pathLen, "%s/%s", basePath, filename);

    return path;
}


bool IsOnSkipList(char* filename)
{
    if (StrCmp(filename, ".git") || StrCmp(filename, ".GIT") )
    {
        return true;
    }
    return false;
}


void CheckKeyLength(char* key)
{
    if (strlen(key) != KEY_LEN) ThrowError("Length must have the length of 32.");
}