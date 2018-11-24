
#include "structs.h"
#include "greedy.h"
#include "data_treatment.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "simulation.h"
#include "greedy_min.h"
int main()
{
	srand(time(NULL));


	simul(&greedy,"greedy");
	simul(&loaded_greedy,"loaded_greedy");
	simul_period(&greedy_PRIME,"greedy_prime");
	simul_period(&greedy_min_lost,"greedy_min");

	char * noms[] = {"greedy","loaded_greedy"};
	print_gnuplot("res",noms, 2, "performance of greedys with waiting times", "tmax", "sucess rate");

	char * noms2[] = {"greedy_prime","greedy_min"};
	print_gnuplot("res2",noms2, 2, "performance of greedys without waiting time", "load", "sucess rate");
	//test();
	return 0;
}