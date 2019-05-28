
#include "structs.h"
#include "greedy_waiting.h"
#include "data_treatment.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "simulation.h"
#include "string.h"
#include "greedy_without_waiting.h"
int main (int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Usage %s [arg]",argv[0]);
		return 1;
	}
	if(!strcmp(argv[1],"test"))
	{
		test();
	}

	else
	{
		int seed = time(NULL);

		simul(seed,&greedy,"greedy");
		simul(seed,&loaded_greedy,"loaded greedy");
		simul(seed,&loaded_greedy_longest,"loaded greedy longest");
		//simul_period(seed,&greedy_PRIME,"greedy prime");
		//simul_period(seed,&greedy_min_lost,"greedy min");

		char * noms[] = {"greedy","loaded greedy","loaded greedy longest"};
		char * ylabels[] = {"greedy","loaded greedy","loaded greedy longest"};
		print_gnuplot("waiting",noms, 3, "performance of greedys with waiting times", "tmax", ylabels);

		/*char * noms2[] = {"greedy prime","greedy min"};
		char * ylabels2[] = {"greedy prime","greedy min"};
		print_gnuplot("nowaiting",noms2, 2, "performance of greedys without waiting time", "load", ylabels2);
		*/
	}
	
	return 0;
}