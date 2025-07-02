#ifndef __ERROR_HANDLE__
#define __ERROR_HANDLE__

#include "cutil.h"
#include <stdexcept>

bool WriteError(char* errorMessage);
void PrintError();
void ErrorClean();
bool CheckForError();
void ThrowError(char* ErrorMessage);
#endif