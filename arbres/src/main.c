#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "structs.h"
#include "greedy_waiting.h"
#include "data_treatment.h"
#include "spall_waiting.h"
#include "simulation.h"
#include "simulation_star.h"
#include "string.h"
#include "greedy_without_waiting.h"
#include "reusePrime.h"
#include "inputonos.h"
#include "voisinage.h"
#include "starSPALL.h"
int main (int argc, char *argv[])
{
	int seed = time(NULL);
	if(argc < 2)
	{
		printf("Usage %s [arg]",argv[0]);
		return 1;
	}
	if(!strcmp(argv[1],"test"))
	{
		test();
	}
	if(!strcmp(argv[1],"onos"))
	{
		parseinput();
	}
	if(!strcmp(argv[1],"star"))
	{
		//star_search_random_routes();
		//star_all_routes_lenghts();
		char * ylabels[] = {"Success Rate","NbRoutes"};
		simul_star(seed,&fpt_spall,"FPTSPALL");
		char * noms[]={"FPTSPALL"};
		print_gnuplot("fptaspall",noms, 1, "performance of fptspall", "margin", ylabels);
	}
	else
	{
		
		char * ylabels[] = {"Success Rate","NbRoutes"};
		if(!strcmp(argv[1],"simulWaiting") || !strcmp(argv[1],"simulAll"))
		{
			/*simul(seed,&greedy,"greedy");
			simul(seed,&loaded_greedy,"loadedGreedy");
			simul(seed,&loaded_greedy_longest,"loadedGreedyLongest");
			simul(seed,&loaded_greedy_collisions,"loadedGreedyCollisions");
			simul(seed,&RRH_first_spall,"RRHFirst");*/
			simul(seed,&descente,"Descente");
			//simul(seed,&best_of_x,"DescenteX");
			char * noms[] = {"greedy","loadedGreedy","loadedGreedyLongest","loadedGreedyCollisions","RRHFirst","Descente","DescenteX"};
			print_gnuplot("waiting",noms, 6, "performance of greedys with waiting times", "margin", ylabels);
		}
		if(!strcmp(argv[1],"simulNoWaiting") || !strcmp(argv[1],"simulAll"))
		{
			simul_period(seed,&PRIME_reuse,"PrimeReuse");
			simul_period(seed,&greedy_PRIME,"greedyPrime");
			//simul_period(seed,&greedy_tics_won,"greedyTicsWon");
			char * noms2[] = {"greedyPrime","PrimeReuse","greedyTicsWon"};
			print_gnuplot("nowaiting",noms2, 3, "performance of greedys without waiting time", "load", ylabels);
		}
		
		

		

		
		
	}
	
	return 0;
}