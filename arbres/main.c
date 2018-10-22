#include "simulation.h"
#include "structs.h"
#include "greedy.h"
#include "data_treatment.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main()
{
	srand(time(NULL));


	simul(&greedy,"greedy");
	simul(&loaded_greedy,"loaded_greedy");

	char * noms[] = {"greedy","loaded_greedy"};
	print_gnuplot("res",noms, 2, "performance of greedys", "tmax", "sucess rate");
	//test();
	return 0;
}