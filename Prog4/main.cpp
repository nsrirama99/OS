#include <iostream>
#include <time.h>
#include "OS.h"


main()
{
   OS os1(10, 1, 5, 1);
   
   for (int i = 0; i < 10; i++)
	{
	   Process p;

	   int steps = rand() % 10 + 5;
   	   for (int j = 0; j < steps; j++)
		{
		   Entry e;
		  
		   e.set(rand() % 10, rand()% 10, rand() %10);
		   p.AppendEntry(e);
		}

	    if (i == 6)
		p.IncrementPC();
	    if (i == 7)
		p.SetPC(3);
	    os1.AddProcess(p);

	}

  cout << os1;

}
