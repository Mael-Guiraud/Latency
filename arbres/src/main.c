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
#include "multiplexing.h"
#include "borneInf.h"
int main (int argc, char *argv[])
{
	unsigned int seed = time(NULL);

	if(argc < 2)
	{
		printf("Usage %s [arg]",argv[0]);
		return 1;
	}
	if(!strcmp(argv[1],"test"))
	{

		
		
		while(1)
		{
				test(seed);
				seed++;
		}
	
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
	
	if(!strcmp(argv[1],"simulDistrib") )
	{

		simuldistrib(seed);
	}
	if(!strcmp(argv[1],"simulFPT") )
	{
		testfpt(seed);
		//testcoupefpt(seed);
	}
	if(!strcmp(argv[1],"simuldescente") )
	{
		simuldescente(seed);
	}
	if(!strcmp(argv[1],"simultaboo") )
	{
		simultaboo(seed);
	}




	

		
		
	
	
	return 0;
}