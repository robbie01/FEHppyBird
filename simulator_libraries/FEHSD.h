#ifndef FEHSD_H
#define FEHSD_H
// #include "SDHC.h"
// #include "ff.h"
// #include "ffconf.h"
// #include "diskio.h"
#include "FEHFile.h"

class FEHSD
{
public:
    FEHSD();
    FEHFile *FOpen(const char *str, const char *mode);
    int FClose(FEHFile *fptr);
    int FCloseAll();
    int FPrintf(FEHFile *fptr, const char *format,
                /* Pointer to the format string */...);
    int FScanf(FEHFile *fptr, const char *format, ...);
    int FEof(FEHFile *fptr);
    //int FSeek(FEHFile *fptr, long int offset, int position);

private:
    int Initialize();
    int isInitialized;
    int numberOfFiles;
};

extern FEHSD SD;
#endif
