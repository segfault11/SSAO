#include "Random.h"
#include <cstdlib>
#include <ctime>

//------------------------------------------------------------------------------
Random::Random ()
{
	srand(time(NULL));
}
//------------------------------------------------------------------------------
Random::~Random ()
{

}
//------------------------------------------------------------------------------
float Random::GetRandomRanged (float s, float e)
{
	int mod = 10000;
	int r = rand() % mod;

	return s + static_cast<float>(r)/static_cast<float>(mod - 1)*(e - s);
	
}
//------------------------------------------------------------------------------
