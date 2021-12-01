#include "FEHRandom.h"

#include <time.h>

FEHRandom Random;

void FEHRandom::Seed()
{
	srand(time(NULL));

	// For some reason, it was observed that the first random number didn't seem to be that random :(
	// So, we fetch a few random numbers for funsies
	// Please don't remove these :)
	RandInt();
	RandInt();
	RandInt();
}

int FEHRandom::RandInt()
{
	// We specify in documentation rand will be between 0 and 32767
	// rand() returns between 0 and RAND_INT, which will be atleast 32767
	// Thus, modulo by 32767 to ensure it will never be above 32767
	return rand() % 32767; // steal rand from stdlib.h
}