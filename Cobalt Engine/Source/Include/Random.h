/**
	Random.h

	"Mersenne Twister pseudorandom number generator"
	Original Code written by Takuji Nishimura and Makoto Matsumoto

	Class taken from Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#define CMATH_N 624
#define CMATH_M 397
#define CMATH_MATRIX_A 0x9908b0df   /* constant vector a */
#define CMATH_UPPER_MASK 0x80000000 /* most significant w-r bits */
#define CMATH_LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */
#define CMATH_TEMPERING_MASK_B 0x9d2c5680
#define CMATH_TEMPERING_MASK_C 0xefc60000
#define CMATH_TEMPERING_SHIFT_U(y)  (y >> 11)
#define CMATH_TEMPERING_SHIFT_S(y)  (y << 7)
#define CMATH_TEMPERING_SHIFT_T(y)  (y << 15)
#define CMATH_TEMPERING_SHIFT_L(y)  (y >> 18)


class RandomGenerator
{
public:
	RandomGenerator();

	unsigned int Random(unsigned int n);
	float Random();
	void SetRandomSeed(unsigned int n);
	unsigned int GetRandomSeed();
	void Randomize();

private:
	unsigned int rseed;
	unsigned int rseed_sp;
	unsigned long mt[CMATH_N];
	int mti;
};