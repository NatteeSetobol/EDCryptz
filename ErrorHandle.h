#ifndef __ERROR_HANDLE__
#define __ERROR_HANDLE__

#include "cutil.h"


bool WriteError(char* errorMessage);
void PrintError();
void ErrorClean();
bool CheckForError();
#endif