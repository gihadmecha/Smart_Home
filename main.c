
#include "Smart_Home.h"

int main ()
{
	Smart_Home_Init ();
	
	while (1)
	{
		Smart_Home_Run ();
	}
}