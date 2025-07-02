#include "ErrorHandle.h"

extern char* errorMsg=NULL;

bool WriteError(char* errorMessage)
{
    int errorLen = 0;

    // If error buffer is already exist then return false   
    if (errorMsg != NULL) return false;

    errorLen = strlen(errorMessage);

    if (errorLen <= 0) return false;

    errorMsg = (char*)Alloc(errorLen*sizeof(char)+10);

    if (errorMsg)
    {
        memcpy(errorMsg, errorMessage,errorLen);
    } else {
        return false;
    }

    return true;
}

void PrintError()
{
    if (errorMsg != NULL)
    {
        printf("%s\n",errorMsg);
    }
}

void ErrorClean()
{
    if (CheckForError())
    {
        free(errorMsg);
        errorMsg=NULL;
    }
}

bool CheckForError()
{
    if (errorMsg)
    {
        return true;
    }
    return false;
}

void ThrowError(char* ErrorMessage)
{
    throw std::runtime_error(ErrorMessage);
}