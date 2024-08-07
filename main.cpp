#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include "ErrorHandle.h"
#include "Encryption.h"

enum typeChoice {
    NONE,
    FILE_TYPE,
    DIRECTORY_TYPE
};


bool EncryptDirectory(char *basePath, uint8_t *key, bool encrypt,char* output);
void ShowOptions();
void Logo();
char* CreatePath(char* basePath, char *filename);

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

                if (stat(optionVal, &info) == -1)
                {
                    WriteError("[-] Error File not found."); }
                if (S_ISDIR(info.st_mode))
                {
                    choice = DIRECTORY_TYPE;
                    filename = optionVal;
                } else {
                    choice = FILE_TYPE;
                    filename = optionVal;
                }
            }

            if (StrCmp(option, "-o"))
            {
                outputFile = optionVal;
            }
            if (StrCmp(option, "-k"))
            {
                /*
                format A0B0C0E0F0010203040506070809A1A2
                Check if the key length is 32
                allocate space for new key unit8_t* = key uint8_t[32]
                for loop by 2 to get hex and convert to uint_8
                */

                if (strlen(optionVal) == 32)
                {
                    int keyIndex = 0;

                    //key = (uint8_t*) optionVal ;
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
                } else {
                    WriteError("[-] Key should be exactly 32 in length.\n");
                }
            }
        } 
    }

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
                    if (outputFile)
                    {
                        mkdir(outputFile, 0755);
                        printf("[+] Encrypting Directory: %s to %s \n", filename,outputFile);
                    } else {
                        printf("[+] Encrypting Directory: %s \n", filename);
                    }
                    EncryptDirectory(filename,key, true, outputFile);
                    printf("[+] Encrypted Directory.");
                } else {
                    printf("[+] Encrypting file: %s\n", filename);
                    EncryptFile(filename, key, outputFile);
                    printf("[+] Encrypted file.\n");
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
                        if (outputFile)
                        {
                            mkdir(outputFile, 0755);
                            printf("[+] Decrypting Directory: %s to %s\n", filename,outputFile);
                        } else {
                            printf("[+] Decrypting Directory: %s\n", filename);
                        }

                        if (EncryptDirectory(filename,key, false,outputFile))
                        {
                            printf("[+] Successfully decryped directory\n");
                        }
                    } else {
                        printf("[+] Decrypting files\n");
                        DecryptFile(filename, key, outputFile);
                        printf("[+] Decrypted files\n");
                    }
                    //printf("\nYour key is: \n");
                   // PrintHex(key, 16);
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
                        //FIX(): If output is null then this crashes
                        if (output)
                        {
                            folderPath = CreatePath(output,entry->d_name);

                            printf("Created folder %s\n", folderPath);

                            mkdir(folderPath,0755);
                        }

                        success = EncryptDirectory(path,key, encrypt, folderPath);
                    } else {
                        int fullPathLength=0;

                        fullPathLength = strlen(path) + strlen(entry->d_name) + 2;

                        if (fullPathLength > 2)
                        {
                            char *fullPath = NULL;
                            char *outputFile = NULL;

                            fullPath = CreatePath(basePath,entry->d_name);

                            if (output)
                            {
                                outputFile = CreatePath(output,entry->d_name);
                            }
                            if (encrypt)
                            {
                                printf("Full Path: %s\n", fullPath);
                                printf("Output Path: %s\n", outputFile);
                                EncryptFile(fullPath, key,outputFile);
                            } else {
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