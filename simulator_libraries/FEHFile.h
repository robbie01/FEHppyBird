#ifndef FEHFILE_H
#define FEHFILE_H

#include <stdio.h>

class FEHFile
{
public:
	FILE* wrapper;
	int fileIdNum;
	static int numberOfFiles;
	FEHFile()
	{
		fileIdNum = numberOfFiles++;
	}
};
#endif
