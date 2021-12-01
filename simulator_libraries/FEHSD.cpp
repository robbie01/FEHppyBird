#include "FEHSD.h"
#include "FEHLCD.h"
#include <cstdarg>
#include <cstdio>
#include <iostream>

using namespace std;

FEHSD SD;
FEHFile *filePtrs[25];
int FEHFile::numberOfFiles = 0;

FEHSD::FEHSD()
{
    // This function is only present to make the code compile and match the header file
}

FEHFile *FEHSD::FOpen(const char *str, const char *mode)
{
    FEHFile *fehFile = new FEHFile();

    FILE *diskFile = fopen(str, mode);

    // Sets the entire fehFile object to NULL if the disk file is NULL, otherwise populates fehFile.
    if (diskFile == NULL)
    {
        fehFile = NULL;
    }
    else
    {
        fehFile->wrapper = diskFile;
        filePtrs[SD.numberOfFiles++] = fehFile;
    }

    return fehFile;
}

int FEHSD::FClose(FEHFile *fptr)
{
    int retVal = -1;

    if (fptr != NULL)
    {
        // Search through filePtrs to find the given pointer
        for (int i = 0; i < SD.numberOfFiles; i++)
        {
            if (fptr->fileIdNum == (filePtrs[i])->fileIdNum)
            {
                retVal = fclose(filePtrs[i]->wrapper);

                //Shift all elements in array one over to the left
                SD.numberOfFiles--;

                for (int j = i; j < SD.numberOfFiles; j++)
                {
                    filePtrs[j] = filePtrs[j + 1];
                }

                filePtrs[SD.numberOfFiles] = NULL;

                break;
            }
        }
    }

    return retVal;
}

int FEHSD::FCloseAll()
{
    int retVal = 0;

    // Closes any non-null file pointer in filePtrs
    for (int i = 0; i < SD.numberOfFiles; i++)
    {
        if (filePtrs[i] != NULL)
        {
            retVal = fclose(filePtrs[i]->wrapper);

            filePtrs[i] = NULL;
        }
    }
    SD.numberOfFiles = 0;

    return retVal;
}

int FEHSD::FPrintf(FEHFile *fptr, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    // Variable args version of fprintf
    int numChars = vfprintf(fptr->wrapper, format, args);

    va_end(args);

    if (ferror(fptr->wrapper) != 0)
    {
        LCD.WriteLine("Error printing to file");
        return -1;
    }

    // Return number of characters printed
    return numChars;
}

int FEHSD::FScanf(FEHFile *fptr, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    // Check for end of file, return -1 if eof
    if (feof(fptr->wrapper))
    {
        return -1;
    }

    // Scan line and store in args; also get number of args read
    int numRead = vfscanf(fptr->wrapper, format, args);

    va_end(args);

    // Return number of successful reads
    return numRead;
}

int FEHSD::FEof(FEHFile *fptr)
{
    return feof(fptr->wrapper);
}