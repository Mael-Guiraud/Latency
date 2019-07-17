#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "structs.h"
#include "greedy_waiting.h"
#include "data_treatment.h"
#include "spall_waiting.h"
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
	if(!strcmp(argv[1],"star"))
	{
		star();
	}
	else
	{
		int seed = time(NULL);
		char * ylabels[] = {"Success Rate","NbRoutes"};
		if(!strcmp(argv[1],"simulWaiting") || !strcmp(argv[1],"simulAll"))
		{
			simul(seed,&greedy,"greedy");
			simul(seed,&loaded_greedy,"loadedGreedy");
			simul(seed,&loaded_greedy_longest,"loadedGreedyLongest");
			simul(seed,&loaded_greedy_collisions,"loadedGreedyCollisions");
			simul(seed,&RRH_first_spall,"RRHFirst");
			char * noms[] = {"greedy","loadedGreedy","loadedGreedyLongest","loadedGreedyCollisions","RRHFirst"};
			print_gnuplot("waiting",noms, 5, "performance of greedys with waiting times", "margin", ylabels);
		}
		if(!strcmp(argv[1],"simulNoWaiting") || !strcmp(argv[1],"simulAll"))
		{
			simul_period(seed,&greedy_PRIME,"greedyPrime");
			simul_period(seed,&greedy_tics_won,"greedyTicsWon");
			char * noms2[] = {"greedyPrime","greedyTicsWon"};
			print_gnuplot("nowaiting",noms2, 2, "performance of greedys without waiting time", "load", ylabels);
		}
		
		

		

		
		
	}
	
	return 0;
}