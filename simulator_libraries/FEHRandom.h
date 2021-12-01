#ifndef FEHRANDOM_H
#define FEHRANDOM_H

#include <stdlib.h>

class FEHRandom
{
public:
	void Seed();
	int RandInt();
};

extern FEHRandom Random;

#endif // FEHRANDOM_H
