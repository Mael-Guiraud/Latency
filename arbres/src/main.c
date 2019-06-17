#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "structs.h"
#include "greedy_waiting.h"
#include "data_treatment.h"

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
		simul(seed,&loaded_greedy,"loaded_greedy");
		simul(seed,&loaded_greedy_longest,"loaded_greedy_longest");
		simul(seed,&loaded_greedy_collisions,"loaded_greedy_collisions");

		simul_period(seed,&greedy_PRIME,"greedy_prime");
		simul_period(seed,&greedy_tics_won,"greedy_tics_won");

		char * noms[] = {"greedy","loaded_greedy","loaded_greedy_longest","loaded_greedy_collisions"};
		char * ylabels[] = {"greedy","loaded greedy","loaded greedy longest","loaded greedy collisions"};
		print_gnuplot("waiting",noms, 4, "performance of greedys with waiting times", "tmax", ylabels);

		char * noms2[] = {"greedy prime","greedy tics won"};
		char * ylabels2[] = {"greedy prime","greedy tics won"};

		print_gnuplot("nowaiting",noms2, 2, "performance of greedys without waiting time", "load", ylabels2);
		
	}
	
	return 0;
}