
#include "structs.h"
#include "greedy_waiting.h"
#include "data_treatment.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "simulation.h"
#include "greedy_without_waiting.h"
int main()
{
	
	/*int seed = time(NULL);
	


	simul(seed,&greedy,"greedy");
	simul(seed,&loaded_greedy,"loaded greedy");
	simul_period(seed,&greedy_PRIME,"greedy prime");
	simul_period(seed,&greedy_min_lost,"greedy min");

	char * noms[] = {"greedy","loaded greedy"};
	char * ylabels[] = {"greedy","loaded greedy"};
	print_gnuplot("waiting",noms, 2, "performance of greedys with waiting times", "tmax", ylabels);

	char * noms2[] = {"greedy prime","greedy min"};
	char * ylabels2[] = {"greedy prime","greedy min"};
	print_gnuplot("nowaiting",noms2, 2, "performance of greedys without waiting time", "load", ylabels2);
	*/
	test();
	return 0;
}