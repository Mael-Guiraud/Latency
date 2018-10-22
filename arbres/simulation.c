#include "structs.h"
#include "test.h"
#include "init.h"
#include "treatment.h"
#include "config.h"
#include "greedy.h"
#include "loaded_simons.h"

#include <stdio.h>
void test()
{
	
	int P = PERIOD;
	int message_size = MESSAGE_SIZE;
	Graph g = init_graph_random_tree(P);
	affiche_graph(g,P);
	printf("Liens load : ");
	affiche_tab(load_links(g),g.arc_pool_size);

	Assignment a = loaded_greedy( g, P, message_size);
	if(a)
	{
		affiche_assignment( a,g.nb_routes);
		free_assignment(a);
	}
	else
		printf("No assignment found\n");
	affiche_graph(g,P);
	free_graph(g);

}

void simul(Assignment (*ptrfonction)(Graph,int,int),char * nom)
{

	int message_size = MESSAGE_SIZE;
	int nb_success;
	Graph g;
	int P = PERIOD;
	FILE * f = fopen(nom,"w");
	if(!f)perror("Error while opening file\n");
	for(int tmax=TMAX_MIN;tmax<=TMAX_MAX;tmax+=TMAX_GAP)
	{
		nb_success=0;
		for(int i=0;i<NB_SIMULS;i++)
		{
			g= init_graph_random_tree(P);
			Assignment a = ptrfonction( g, P, message_size);
			if(a)
			{
				if(travel_time_max( g, tmax, a))
				{

					nb_success++;
					free_assignment(a);
				}
			}
			free_graph(g);
			fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
		}	
		printf("\n TMAX : %d success : %d/%d (Loadmax = %f).\n",tmax,nb_success,NB_SIMULS,(load_max(g)*MESSAGE_SIZE)/(float)PERIOD);
		fprintf(f,"%d %f\n",tmax,nb_success/(float)NB_SIMULS);
	}
	
		
}